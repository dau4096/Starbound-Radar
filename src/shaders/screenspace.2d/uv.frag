/* uv.frag */
#version 460 core

in vec2 fragUV;
out vec4 fragColour;

void main() {
	//Simply outputs the pixel's screen UV.
	fragColour = vec4(fragUV.xy, 0.0f, 1.0f);
}
