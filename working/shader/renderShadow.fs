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


uniform vec3 lightPos;
uniform vec3 viewPos;

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

	int range = 4;
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

	// calculate shadow
	float shadow = ShadowCalculation( fs_in.FragPosLightSpace, max(0.05 * (1.0 - dot(normal, lightDir)), 0.005) );
	vec3 lighting = (ambient + ( 1.0 - shadow) * ( diffuse + specular )) * color ;

	FragColor = vec4( lighting, 1.0 );
}