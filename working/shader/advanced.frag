#version 330 core

in vec2 TexCoords;
in vec3 Position;
in vec3 Normal ;

out vec4 color;

uniform sampler2D texture1;
uniform samplerCube skybox ;

uniform vec3 cameraPos ;

void main()
{             
	vec4 texColor = texture(texture1, TexCoords);
    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
	vec4 ref = texture( skybox, R ) ;
	if ( texColor.a < 0.1 )
		discard ;
    color = texColor + ref*0.3;
}