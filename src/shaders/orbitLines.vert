/* orbitLines.vert */
#version 460 core

layout(location=0) in vec2 aPos;
out vec2 fragPosition;


uniform ivec2 centre;
uniform float radius;
uniform float scaling;
uniform ivec2 offset;
uniform mat4 projectionMatrix;
uniform ivec2 resolution;


void main() {
    vec2 pos = centre + (aPos * radius);
    gl_Position = projectionMatrix * vec4(pos.xy * scaling + offset + (resolution / 2), 0.0f, 1.0f);
    fragPosition = pos;
}

