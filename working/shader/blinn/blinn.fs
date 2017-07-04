#version 330 core
out vec4 FragColor ;


in VS_OUT {
	vec3 FragPos ;
	vec3 Normal ;
	vec2 Texcoords;
} fs_in;

uniform sampler2D mainTex ;
uniform vec3 lightPos ;
uniform vec3 viewPos;
uniform bool blinn ;
uniform bool gamma;

void main() {
	vec3 color = texture( mainTex, fs_in.Texcoords ).rgb;
	vec3 ambient = 0.05 * color ;
	vec3 lightDir = normalize( lightPos - fs_in.FragPos );
	vec3 normal = normalize( fs_in.Normal );

	float diff = max( dot( normal, lightDir ), 0.0 );
	vec3 diffuse = diff * color ;
	diffuse = pow( diffuse, vec3(2.2) );

	vec3 viewDir = normalize( viewPos - fs_in.FragPos ) ;
	vec3 reflectDir = reflect( -lightDir, normal );

	float spec = 0.0;
	if ( blinn ) {
		vec3 halfwayDir = normalize( lightDir + viewDir );
		spec = pow( max( dot( normal, halfwayDir ), 0 ), 32.0 );
	}else {
		vec3 reflectDir = reflect(-lightDir, normal);
		spec = pow( max( dot( normal, reflectDir ), 0 ), 32.0 );
	}

	float distance = length(lightPos - fs_in.FragPos);
	float attenuation = 1.0 / (gamma ? distance * distance : distance);

	vec3 specular = vec3( 0.5 ) * spec ;
	color =  ambient + (diffuse + specular) * attenuation ;
	if ( gamma ){
		color = pow( color, vec3(1.0/2.2) );
	}

	FragColor = vec4( color, 1.0 );
}