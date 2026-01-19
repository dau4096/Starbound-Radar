/* worldspace.vert */
#version 460 core

out vec2 fragUV;
//out vec3 fragNormal; //Normals optional.

layout(location=0) vec3 aPos;
layout(location=1) vec2 aUV;
//layout(location=2) vec3 aNormal; //Optional.

uniform mat4 pvmMatrix;

void main() {
	gl_Position = (pvmMatrix * vec4(aPos, 1.0f)).xyz;
	fragUV = aUV;
	//fragNormal = aNormal; //Optional.
}
