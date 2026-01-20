/* orbitLines.frag */
#version 460 core

out vec4 fragColour;
in vec2 fragPosition;

uniform ivec2 bodyPosition;
uniform ivec2 centre;
uniform vec3 colour;

#define MAX_RANGE 0.125f
#define BASE_COLOUR 0.125f

void main() {
	double dotProd = dot(
		normalize(dvec2(fragPosition - centre)), //Direction from centre of circle to this fragment.
		normalize(dvec2(bodyPosition - centre)) //Direction from centre of circle to the body.
	);
	float a = float((dotProd - 1.0f + MAX_RANGE) / MAX_RANGE);
	float c = clamp(a, 0.0f, 1.0f);
	fragColour = vec4(mix(vec3(BASE_COLOUR, BASE_COLOUR, BASE_COLOUR), colour.rgb, c), 1.0f); //Grey gradient depending how close to the body's angle it is.
}
