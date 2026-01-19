#pragma once

#include "includes.h"
#include <glm/glm.hpp>




namespace constants {
	//Mathematical Constants
	constexpr float PI = glm::pi<float>();
	constexpr float PI2 = PI * 2.0f;
	constexpr float EXP = glm::e<float>();
	constexpr float INF = std::numeric_limits<float>::infinity();

	constexpr float TO_RAD = 0.017453f;
	constexpr float TO_DEG = 57.29577f;


	//Exct.
	constexpr size_t NUMBER_OF_BODIES_TO_RESERVE = 128u;
	constexpr size_t NUMBER_OF_ROUTES_TO_RESERVE = 32u;
	constexpr size_t NUMBER_OF_SSHIPS_TO_RESERVE = 16u;
}

namespace sim {
	//Simulation constants;
	constexpr float SHIP_Gs = 1.0f;
	constexpr float C = 299792.0f;

	//Scaling constants
	constexpr unsigned int PERIOD_MULTIPLIER = 86400u; //86,400 seconds in a day.
	constexpr unsigned int SCALE_MULTIPLIER = 1000u; //Megametres, 1 unit is 1km.
	constexpr float TIME_PRECISION = 0.125f; //Precision to 1/8ths.
}

namespace display {
	//Resolutions
	constexpr glm::ivec2 WINDOW_RESOLUTION = glm::ivec2(1280, 720);
	constexpr glm::ivec2 RENDER_RESOLUTION = glm::ivec2(1280, 720);

	//Opengl 460 core.
	constexpr int OPENGL_VERSION_MAJOR = 4;
	constexpr int OPENGL_VERSION_MINOR = 6;

	//Texture Standardisation
	constexpr glm::ivec2 TEXTURE_RESOLUTION = glm::ivec2(128, 128);

	//Time
	constexpr double HZ = 60.0d;
	constexpr double DT = 1.0f/HZ;
}

namespace dev {
	//Assorted DEV/DEBUG constants
	constexpr bool PAUSE_ON_OPENGL_ERROR = true;
	constexpr bool SHOW_HZ_CONSOLE = false;
	constexpr bool SHOW_DT_CONSOLE = false;
	constexpr bool VSYNC = false;

	//Debug
	//Loader;
	constexpr bool SHOW_HEIRARCHY_CONSOLE = false;
	constexpr bool SHOW_ROUTES_CONSOLE = false;
	constexpr bool SHOW_SHIPS_CONSOLE = false;
	//Etc;
	constexpr bool DEBUG_BODY_LOCATIONS = true;
}