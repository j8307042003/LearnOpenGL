#version 330 core

in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;


void main(){

	float lightDis = length( FragPos.xyz - lightPos );
	lightDis = lightDis / (far_plane );
	gl_FragDepth =  lightDis;
}