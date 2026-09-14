#version 410 core

in vec4 vColour;

out vec4 fragColour;

uniform float uTime;

void main() {
    vec3 shift = vec3(
        sin(uTime) * 0.5 + 0.5,
        sin(uTime + 3.0) * 0.5 + 0.5,
        sin(uTime + 5.0) * 0.5 + 0.5
    );

    fragColour = vec4(vColour.rgb * shift, vColour.a);
}