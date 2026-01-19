/* sprite.vert */
#version 460 core


out vec2 fragUV;

uniform ivec2 centre;
uniform uint radius;
uniform float scaling;
uniform ivec2 offset;
uniform mat4 projectionMatrix;


const vec2 v[4] = {
	vec2(-1.0f, -1.0f),
	vec2( 1.0f, -1.0f),
	vec2(-1.0f,  1.0f),
	vec2( 1.0f,  1.0f),
};

void main() {
    vec2 pos = centre + (v[gl_VertexID] * float(radius));
    gl_Position = projectionMatrix * vec4(pos.xy * scaling + offset, 0.0f, 1.0f);
	fragUV = clamp(v[gl_VertexID], 0.0f, 1.0f);
}
