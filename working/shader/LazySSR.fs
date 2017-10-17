#version 330 core
out vec4 FragColor;

in VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;
uniform sampler2D colorTexture;
uniform sampler2D depthMap;

uniform mat4 projectionView;
uniform float near_plane;
uniform float far_plane;

uniform mat4 projection;
uniform mat4 view;

uniform vec3 lightPos;
uniform vec3 viewPos;

int numSampleStep = 128;








#define point2 vec2
#define point3 vec3
 
float distanceSquared(vec2 a, vec2 b) { a -= b; return dot(a, a); }
 
// Returns true if the ray hit something
bool traceScreenSpaceRay1(
 // Camera-space ray origin, which must be within the view volume
 point3 csOrig, 
 
 // Unit length camera-space ray direction
 vec3 csDir,
 
 // A projection matrix that maps to pixel coordinates (not [-1, +1]
 // normalized device coordinates)
 mat4x4 proj, 
 
 // The camera-space Z buffer (all negative values)
 sampler2D csZBuffer,
 
 // Dimensions of csZBuffer
 vec2 csZBufferSize,
 
 // Camera space thickness to ascribe to each pixel in the depth buffer
 float zThickness, 
 
 // (Negative number)
 float nearPlaneZ, 
 
 // Step in horizontal or vertical pixels between samples. This is a float
 // because integer math is slow on GPUs, but should be set to an integer >= 1
 float stride,
 
 // Number between 0 and 1 for how far to bump the ray in stride units
 // to conceal banding artifacts
 float jitter,
 
 // Maximum number of iterations. Higher gives better images but may be slow
 const float maxSteps, 
 
 // Maximum camera-space distance to trace before returning a miss
 float maxDistance, 
 
 // Pixel coordinates of the first intersection with the scene
 out point2 hitPixel, 
 
 // Camera space location of the ray hit
 out point3 hitPoint) {
 
    // Clip to the near plane    
    float rayLength = ((csOrig.z + csDir.z * maxDistance) > nearPlaneZ) ?
        (nearPlaneZ - csOrig.z) / csDir.z : maxDistance;
    point3 csEndPoint = csOrig + csDir * rayLength;
 
    // Project into homogeneous clip space
    vec4 H0 = proj * vec4(csOrig, 1.0);
    vec4 H1 = proj * vec4(csEndPoint, 1.0);
    float k0 = 1.0 / H0.w, k1 = 1.0 / H1.w;
 
    // The interpolated homogeneous version of the camera-space points  
    point3 Q0 = csOrig * k0, Q1 = csEndPoint * k1;
 
    // Screen-space endpoints
    point2 P0 = H0.xy * k0, P1 = H1.xy * k1;
 
    // If the line is degenerate, make it cover at least one pixel
    // to avoid handling zero-pixel extent as a special case later
    P1 += vec2((distanceSquared(P0, P1) < 0.0001) ? 0.01 : 0.0);
    vec2 delta = P1 - P0;
 
    // Permute so that the primary iteration is in x to collapse
    // all quadrant-specific DDA cases later
    bool permute = false;
    if (abs(delta.x) < abs(delta.y)) { 
        // This is a more-vertical line
        permute = true; delta = delta.yx; P0 = P0.yx; P1 = P1.yx; 
    }
 
    float stepDir = sign(delta.x);
    float invdx = stepDir / delta.x;
 
    // Track the derivatives of Q and k
    vec3  dQ = (Q1 - Q0) * invdx;
    float dk = (k1 - k0) * invdx;
    vec2  dP = vec2(stepDir, delta.y * invdx);
 
    // Scale derivatives by the desired pixel stride and then
    // offset the starting values by the jitter fraction
    dP *= stride; dQ *= stride; dk *= stride;
    P0 += dP * jitter; Q0 += dQ * jitter; k0 += dk * jitter;
 
    // Slide P from P0 to P1, (now-homogeneous) Q from Q0 to Q1, k from k0 to k1
    point3 Q = Q0; 
 
    // Adjust end condition for iteration direction
    float  end = P1.x * stepDir;
 
    float k = k0, stepCount = 0.0, prevZMaxEstimate = csOrig.z;
    float rayZMin = prevZMaxEstimate, rayZMax = prevZMaxEstimate;
    float sceneZMax = rayZMax + 100;
    for (point2 P = P0; 
         ((P.x * stepDir) <= end) && (stepCount < maxSteps) &&
         ((rayZMax < sceneZMax - zThickness) || (rayZMin > sceneZMax)) &&
          (sceneZMax != 0); 
         P += dP, Q.z += dQ.z, k += dk, ++stepCount) {
         
        rayZMin = prevZMaxEstimate;
        rayZMax = (dQ.z * 0.5 + Q.z) / (dk * 0.5 + k);
        prevZMaxEstimate = rayZMax;
        if (rayZMin > rayZMax) { 
           float t = rayZMin; rayZMin = rayZMax; rayZMax = t;
        }
 
        hitPixel = permute ? P.yx : P;
        // You may need hitPixel.y = csZBufferSize.y - hitPixel.y; here if your vertical axis
        // is different than ours in screen space
        sceneZMax = texelFetch(csZBuffer, ivec2(hitPixel), 0).r;
    }
     
    // Advance Q based on the number of steps
    Q.xy += dQ.xy * stepCount;
    hitPoint = Q * (1.0 / k);
    return (rayZMax >= sceneZMax - zThickness) && (rayZMin < sceneZMax);
}





// required when using a perspective projection matrix
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}

float ShadowCalculation(vec4 fragPosLightSpace, float eps) {
	//float eps = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005); 
	// perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w; // [-w, w] -> [-1, 1]

	projCoords = projCoords * 0.5 + 0.5; // [-1, 1] -> [0, 1]


	// get shadow depth from shadow Map
	float closestDepth = texture(shadowMap, projCoords.xy).r;


	// get depth of this fragment
	float currentDepth = projCoords.z;
	
	float shadow = 0.0;//(currentDepth - eps > closestDepth)  ? 1.0 : 0.0;


	vec2 texelSize = 1.0 / textureSize( shadowMap, 0 );

	int range = 2;
	for ( int x = -range ; x <= range ; ++x ) {
		for ( int y = -range ; y <= range ; ++y ) {
			float pcfDepth = texture( shadowMap, projCoords.xy + vec2(x, y) * texelSize ).r;
			shadow += (currentDepth - eps > pcfDepth) ? 1.0 : 0.0;
		}
	}

	shadow /= pow((range*2 + 1 ), 2);

	if (projCoords.z > 1.0)
		shadow = 0.0 ;

	// openGL Z axis
	return shadow;
}

void main(){
	
	vec3 color = texture(diffuseTexture, fs_in.TexCoords ).rgb;
	vec3 normal = normalize(fs_in.Normal);
	vec3 lightColor = vec3(1.0);

	//ambient
	vec3 ambient = 0.15 * color ;

	//diffuse
	vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	float diff = max(dot(lightDir, normal), 0.0 );
	vec3 diffuse = diff * lightColor;

	//specular
	vec3 viewDir = normalize( viewPos - fs_in.FragPos );
	float spec = 0.0;
	vec3 halfwayDir = normalize(lightDir + viewDir);
	spec = pow( max(dot(normal, halfwayDir ), 0.0), 64.0 );
	vec3 specular = spec * lightColor;



	vec4 reflectionVec = normalize(projectionView * vec4( reflect(viewDir, normal), 1.0 ));
	vec2 texelSize = 1.0 / textureSize( depthMap, 0 );
	
	vec3 offset = vec3(reflectionVec.xy*texelSize, 0.0);
	vec3 currPosition = fs_in.FragPos + vec3(reflectionVec.xyz);

	vec4 SSRColor = vec4(0.0,0.0,0.0,0.0);
	for ( int currStep = 0 ; currStep < numSampleStep ; ++ currStep ) {
		vec3 depthSample = texture( depthMap, currPosition.xy ).rgb;
		if ( fs_in.FragPos.z - depthSample.r < 0.0005 ){
		//if ( currPosition.z - depthSample.r < 0.0005 ){
			SSRColor = texture( colorTexture, currPosition.xy );
			//SSRColor = vec4((1.0* currStep) / numSampleStep,0.0,0.0,0.0);
			break;
		}

		currPosition += offset ;		
	}


	// calculate shadow
	float shadow = ShadowCalculation( fs_in.FragPosLightSpace, max(0.05 * (1.0 - dot(normal, lightDir)), 0.005) );
	vec3 lighting = (ambient + ( 1.0 - shadow) * ( diffuse + specular )) * color ;

	FragColor = vec4( lighting + SSRColor.rgb, 1.0 );
	//FragColor = vec4( lighting, 1.0 );
}