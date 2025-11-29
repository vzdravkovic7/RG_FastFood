#version 330 core

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inTex;

out vec2 chTex;

uniform vec2 uPos;
uniform float uScale;

void main()
{
    vec2 scaled = inPos * uScale;
    vec2 finalPos = scaled + uPos;

    gl_Position = vec4(finalPos, 0.0, 1.0);
    chTex = inTex;
}
