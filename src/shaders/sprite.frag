/* sprite.frag */
#version 460 core

uniform sampler2D spriteTexture;

in vec2 fragUV;
out vec4 fragColour;


void main() {
	fragColour = vec4(fragUV.xy, 0.0f, 1.0f);
}
