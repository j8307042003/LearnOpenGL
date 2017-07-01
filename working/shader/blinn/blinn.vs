#version 330 core

layout (location=0) in vec3 pos;
layout (location=1) in vec3 normal;
layout (location = 2) in vec2 texcoords;


out VS_OUT {
	vec3 FragPos ;
	vec3 Normal ;
	vec2 Texcoords;
} vs_out;


uniform mat4 projection ;
uniform mat4 view ;


void main() {
	vs_out.Normal = normal ;
	vs_out.Texcoords = texcoords;
	vs_out.FragPos = pos ;
	gl_Position = projection * view * vec4( pos, 1.0 );

}