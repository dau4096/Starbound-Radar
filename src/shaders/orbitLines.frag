/* orbitLines.frag */
#version 460 core

out vec4 fragColour;
in vec2 fragPosition;

uniform ivec2 bodyPosition;
uniform ivec2 centre;

#define MAX_RANGE 0.125f
#define BASE_COLOUR 0.125f

void main() {
	double dotProd = dot(
		normalize(dvec2(fragPosition - centre)), //Direction from centre of circle to this fragment.
		normalize(dvec2(bodyPosition - centre)) //Direction from centre of circle to the body.
	);
	float a = float((dotProd - 1.0f + MAX_RANGE) / MAX_RANGE);
	float c = clamp(a+BASE_COLOUR, BASE_COLOUR, 1.0f);
	fragColour = vec4(c, c, c, 1.0f); //Grey gradient depending how close to the body's angle it is.
}
