#version 330 core

in vec2 chTex;
out vec4 outCol;

uniform sampler2D uTex;
uniform sampler2D uTex1;
uniform bool useTex1;
uniform float uBlend; // 0.0 -> uTex, 1.0 -> uTex1

void main()
{
    vec4 col1 = texture(uTex, chTex);
    if (!useTex1) {
        outCol = col1;
        return;
    }
    vec4 col2 = texture(uTex1, chTex);

    // pattie blend
    if (col2.a == 0.0) {
        outCol = col1;
    } else {
        outCol = mix(col1, col2, clamp(uBlend, 0.0, 1.0));
    }
}
