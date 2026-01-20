#include "includes.h"
#include "global.h"
#include "utils.h"
using namespace std;
using namespace glm;



/* -------------------------------------------------------------------------------- *\
UTC uses current time to calculate vessel/body positions.
Assumes the default state listed in the data file is the state when UTC value was "0".
Everything operates in real-time, Very deterministic.
\* -------------------------------------------------------------------------------- */


void calculateBody(time_t UTC, structs::CelestialBody* parentBody, structs::CelestialBody* thisBody) {
	//Calculate position around its parent via the current time in UTC.
	//TBA
	glm::vec2 centreOfRotation = parentBody->position;
	double days = static_cast<double>(
		UTC % static_cast<unsigned int>(ceil(thisBody->orbitalPeriod/sim::TIME_PRECISION))
	);
	double a = (days / thisBody->orbitalPeriod) * 2.0f * constants::PI;
	glm::vec2 offset = glm::vec2(cos(a), sin(a)) * thisBody->orbitalRadius;
	thisBody->position = centreOfRotation + offset;
	if (dev::DEBUG_BODY_LOCATIONS) {std::cout << thisBody->name << " : (" << thisBody->position.x << ", " << thisBody->position.y << ")" << std::endl;}

	for (structs::CelestialBody* child : thisBody->children) {
		//Recurse until all children and sub-children are calculated.
		calculateBody(UTC, thisBody, child);
	}
}



namespace bodies {

void evaluate() {
	//Get positions and other data for every object in data::bodies.
	time_t UTC = utils::getTimestamp(); //Get current UTC time (seconds)
	for (structs::CelestialBody& body : data::bodies) {
		if (body.hasParentBody) {continue; /* Do not calculate smaller bodies at this "level". Only evaluate the biggest bodies. */}
		//Body is static, Do not simulate an orbit.
		//Still calculate its children, however.
		for (structs::CelestialBody* child : body.children) {
			//Recurse until all children and sub-children are calculated.
			calculateBody(UTC, &body, child);
		}
	}
	if (dev::DEBUG_BODY_LOCATIONS) {std::cout << std::endl;}
}

}


namespace spacecraft {
	
void evaluate() {
	//Get positions and other data for every ship in data::spacecraft.
	time_t UTC = utils::getTimestamp(); //Get current UTC time (seconds)

}

}