#pragma once
#include "includes.h"
#include "constants.h"
using namespace std;


inline GLFWwindow* Window;


inline std::unordered_map<int, bool> keyMap = {
	//GLFW Enums mapped to boolean values (True if pressed.)
	{GLFW_KEY_ESCAPE, false},
};
inline glm::dvec2 cursorPosition, cursorPositionPrevious, cursorDelta;


inline glm::ivec2 currentWindowResolution;
inline glm::ivec2 currentRenderResolution;
inline float frameRate;
inline unsigned int frameNumber;


inline float globalScaling = 1.0e-6f;     //Camera zoom
inline glm::ivec2 globalOffset = display::RENDER_RESOLUTION / 2; //Camera translation


namespace GLIndex {

//Any indices required for OpenGL stuff.
inline GLint genericVAO;
inline GLuint r1CircleVAO, r1CircleVBO, orbitLineShader, spriteShader;
inline glm::mat4 projectionMatrix;

}




enum CelestialType {
	CT_INVALID,   //Not a valid type.
	CT_STAR,      //Anything stationary.
	CT_PLANET,    //Anything orbiting a star.
	CT_SATELLITE, //Moons, Stations, anything orbiting a planet.
	CT_GATE       //Gates to other stars. Transports ships "instantly" (like teleporting but better)
};


namespace structs {


//Star/Planet/Satellite
struct CelestialBody {
	std::string name;    	//Body name.
	CelestialType type;		//What sort of body?
	glm::ivec2 position;  	//Current position.
	unsigned int radius;	//Radius of the body.

	bool hasParentBody;		//Should orbit around some parent body?
	CelestialBody* parent;	//Star to orbit around.
	float orbitalRadius; 	//Distance from centre to orbit.
	float orbitalPeriod; 	//Time for 1 orbit.
	float progress;			//0-1 of orbit completed.

	std::vector<CelestialBody*> children; //Child bodies.

	CelestialBody() : name("<BODY_INVALID>"), type(CT_INVALID), position(0.0f, 0.0f), radius(0.0f), hasParentBody(false), parent(nullptr), orbitalRadius(0.0f), orbitalPeriod(0.0f), children() {}
	CelestialBody(std::string n, CelestialType t, glm::vec2 pos, unsigned int bR, float oR, float p, CelestialBody* parent=nullptr)
		 : name(n), type(t), position(pos), radius(bR), hasParentBody(parent != nullptr), parent(parent), orbitalRadius(oR), orbitalPeriod(p), children() {}
};


//Static route information.
struct Route {
	std::string number; 					//E.g. "BTN-7274"
	std::vector<CelestialBody*> locations;  //List of places to go.

	Route() : number("<ROUTE_INVALID>"), locations() {}
	Route(std::string n, std::vector<CelestialBody*> l)
		 : number(n), locations(l) {}
};


//Contains flight data.
struct Flight {
	CelestialBody* startBody;	//Start
	glm::ivec2 startPos; 		//Where was the start when the journey began?
	CelestialBody* endBody;		//Destination
	glm::ivec2 endPos;			//Where will it intercept the destination?
	unsigned int ETA; 			//UTF ETA.
	float progress;     		//0-1 of journey completed. Based on time, NOT distance.
	std::string number; 		//E.g. "BTN-7274"

	Flight() : startBody(nullptr), endBody(nullptr), number("<FLIGHT_INVALID>") {}
	Flight(CelestialBody* s, CelestialBody* e, std::string n)
		 : startBody(s), endBody(e), number(n) {}
};


//A single spacecraft.
struct SpaceCraft {
	std::string name;    //Spacecraft name.
	Flight journey;      //Current journey.
	Route* route;	     //The route it follows.
	float speed;	     //Current speed.
	glm::ivec2 position; //Current position.

	SpaceCraft() : name("<SHIP_INVALID>"), journey(), route(nullptr), speed(0.0f) {}
	SpaceCraft(std::string n, Route* r)
		 : name(n), journey(), route(r), speed(0.0f) {}

	float& getSpeed() {
		glm::ivec2 delta = this->journey.endPos - this->journey.startPos;
		float distance = sqrt((delta.x*delta.x) + (delta.y+delta.y)); //glm::length doesn't accept ivec for some reason.
		float progress = this->journey.progress;
		if (progress < 0.5f) {
			//Accelerating
			this->speed = sim::SHIP_Gs * progress * distance;
		} else {
			//Deccelerating
			this->speed = (distance * sim::SHIP_Gs) * (1.0f - progress);
		}
		return this->speed;
	}

	glm::ivec2& getPosition() {
		//Calculate current position given the progress through the journey.
		return this->position;
	}
};


}



//All sim data.
namespace data {
	inline std::vector<structs::CelestialBody> bodies = {};
	inline std::vector<structs::Route> routes = {};
	inline std::vector<structs::SpaceCraft> spacecraft = {};
}


