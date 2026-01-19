#pragma once
#include "includes.h"
#include "constants.h"
using namespace std;


inline GLFWwindow* Window;


inline std::unordered_map<int, bool> keyMap = {
	//GLFW Enums mapped to boolean values (True if pressed.)
	{GLFW_KEY_ESCAPE, false}, //Example
};
inline glm::dvec2 cursorPosition, cursorPositionPrevious, cursorDelta;


inline glm::ivec2 currentWindowResolution;
inline glm::ivec2 currentRenderResolution;
inline float frameRate;
inline unsigned int frameNumber;


namespace GLIndex {

//Any indices required for OpenGL stuff.
inline GLint genericVAO;

}




enum CelestialType {
	CT_INVALID;
	CT_STAR;
	CT_PLANET;
	CT_MOON;
};


namespace structs {


//Star/Planet/Moon
struct CelestialBody {
	std::string name;    	//Body name.
	CelestialType type;		//What sort of body?
	glm::vec2 position;  	//Current position.

	bool hasParentBody;		//Should orbit around some parent body?
	CelestialBody* parent;	//Star to orbit around.
	float orbitalRadius; 	//Distance from centre to orbit.
	float orbitalPeriod; 	//Time for 1 orbit.
	float progress;			//0-1 of orbit completed.

	CelestialBody() : name("<INVALID>"), type(CT_INVALID), position(0.0f, 0.0f), hasParentBody(false), parent(nullptr), orbitalRadius(0.0f), orbitalPeriod(0.0f) {}
	CelestialBody(std::string n, CelestialType t, glm::vec2 pos, float r, float p, CelestialBody* pnt=nullptr)
		 : name(n), type(t), position(pos), hasParentBody(pnt != nullptr), parent(pnt), orbitalRadius(r), orbitalPeriod(p) {}
};


//Contains flight data.
struct Flight {
	Planet* startBody;	//Start
	glm::vec2 startPos; //Where was the start when the journey began?
	Planet* endBody;	//Destination
	glm::vec2 endPos;	//Where will it intercept the destination?
	unsigned int ETA; 	//UTF ETA.
	float progress;     //0-1 of journey completed
	std::string number; //E.g. "BTN-7274"

	Flight() : start(nullptr), end(nullptr), number("<INVALID>") {}
	Flight(Planet* s, Planet* e, std::string n)
		 : start(s), end(e), number(n) {}
};


//A single spacecraft.
struct SpaceCraft {
	std::string name; //Spacecraft name
	Flight journey;   //Current journey
	float speed;	  //Current speed.

	SpaceCraft() : name("<INVALID>"), journey() {}
	SpaceCraft(std::string n)
		 : name(n), journey() {}

	void getSpeed() {
		float distance = this.journey->endPos - this.journey->startPos;
		float progress = this.journey->progress;
		if (progress < 0.5f) {
			//Accelerating
			speed = SHIP_Gs * progress * distance;
		} else {
			//Deccelerating
			speed = (distance * SHIP_Gs) * (1.0f - progress);
		}
	}
};


}
