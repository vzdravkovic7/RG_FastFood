#version 330 core

in vec2 chTex;
out vec4 outCol;

uniform sampler2D uTex;
uniform sampler2D uTex1;
uniform bool useTex1;

void main()
{
    vec4 col1 = texture(uTex, chTex);
    vec4 col2 = texture(uTex1, chTex);
    if(useTex1){
        if(col2.a == 0.0){
            outCol = col1;
        } else {
            outCol = col2;
        }
    } else {
        outCol = col1;
    }
} 