#version 330 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D screenTexture;

const float offset = 1.0 / 300;  


void main()
{             
    vec2 offsets[9] = vec2[](
        vec2(-offset, offset),  // top-left
        vec2(0.0f,    offset),  // top-center
        vec2(offset,  offset),  // top-right
        vec2(-offset, 0.0f),    // center-left
        vec2(0.0f,    0.0f),    // center-center
        vec2(offset,  0.0f),    // center-right
        vec2(-offset, -offset), // bottom-left
        vec2(0.0f,    -offset), // bottom-center
        vec2(offset,  -offset)  // bottom-right    
    );


    float kernel_matrix[9] = float [] (
    	    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  

    ) ;

    vec3 sampleColors[9] ;
    for (int i = 0 ; i < 9 ; i++){
    	sampleColors[i] = vec3( texture(screenTexture, TexCoords.st + offsets[i]) );

    }

    vec3 col = vec3(0.0)  ;
    for (int i = 0 ; i < 9 ; i++){
    	col += sampleColors[i] * kernel_matrix[i];
    }

    color = vec4(col, 1.0) ;
}