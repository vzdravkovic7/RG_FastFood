#version 330 core

in vec2 chTex;
out vec4 outCol;

uniform sampler2D uTex;
uniform sampler2D uTex1;
uniform bool useTex1;
uniform float uBlend;   // 0.0 -> uTex, 1.0 -> uTex1
uniform float uAlpha;

void main()
{
    vec4 col1 = texture(uTex, chTex);

    vec4 color;
    if (!useTex1) {
        color = col1;
    } else {
        vec4 col2 = texture(uTex1, chTex);

        if (col2.a == 0.0) {
            color = col1;
        } else {
            color = mix(col1, col2, clamp(uBlend, 0.0, 1.0));
        }
    }

    color.a *= uAlpha;

    outCol = color;
}
