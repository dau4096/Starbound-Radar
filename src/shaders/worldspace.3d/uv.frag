/* uv.frag */
#version 460 core

in vec2 fragUV;
//in vec3 fragNormal; //Optional normals.
out vec4 fragColour;

void main() {
	//Simply shows the UV per pixel on this triangle.
	fragColour = vec4(fragUV.xy, 0.0f, 1.0f);
}
