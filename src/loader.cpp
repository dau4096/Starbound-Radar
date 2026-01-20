#include "includes.h"
#include "global.h"
#include "utils.h"
#include "physics.h"
using namespace std;
using namespace glm;



static inline std::string trim(std::string s) {
	//Removes whitespace.
	auto start = s.find_first_not_of(" \t\n\r");
	auto end   = s.find_last_not_of(" \t\n\r");
	return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}




namespace xml {


static inline glm::vec2 parseVec2(const std::string& str) {
	std::istringstream ss(str);
	glm::vec2 v;
	ss >> v.x >> v.y;
	return v;
};


static inline glm::vec3 parseVec3(const std::string& str) {
	std::istringstream ss(str);
	glm::vec3 v;
	ss >> v.x >> v.y >> v.z;
	return v;
};


static inline int getInt(const pugi::xml_node& node, std::string attrName, int defaultValue=0) {
	pugi::xml_attribute attr = node.attribute(attrName.c_str());
	if (attr) {
		return attr.as_int();
	}
	return defaultValue;
}

static inline float getFloat(const pugi::xml_node& node, std::string attrName, float defaultValue=0.0f) {
	pugi::xml_attribute attr = node.attribute(attrName.c_str());
	if (attr) {
		return attr.as_float();
	}
	return defaultValue;
}

static inline std::string getString(const pugi::xml_node& node, std::string attrName, std::string defaultValue="") {
	pugi::xml_attribute attr = node.attribute(attrName.c_str());
	if (attr) {
		return attr.as_string();
	}
	return defaultValue;
}

static inline bool getBool(const pugi::xml_node& node, std::string attrName, bool defaultValue=false) {
	pugi::xml_attribute attr = node.attribute(attrName.c_str());
	if (attr) {
		std::string attrValue = utils::strToUpper(attr.as_string());
		if (attrValue == "TRUE" || attrValue == "T") {
			return true;
		} else if (attrValue == "FALSE" || attrValue == "F") {
			return false;
		}
	}
	return defaultValue;
}

static inline glm::vec2 getVec2(const pugi::xml_node& node, std::string attrName, glm::vec2 defaultValue=glm::vec2(0.0f, 0.0f)) {
	pugi::xml_attribute attr = node.attribute(attrName.c_str());
	if (attr) {
		return parseVec2(attr.as_string());
	}
	return defaultValue;
}

static inline glm::ivec2 getIVec2(const pugi::xml_node& node, std::string attrName, glm::vec2 defaultValue=glm::vec2(0.0f, 0.0f)) {
	return static_cast<glm::ivec2>(getVec2(node, attrName, defaultValue));
}

static inline glm::vec3 getVec3(const pugi::xml_node& node, std::string attrName, glm::vec3 defaultValue=glm::vec3(0.0f, 0.0f, 0.0f)) {
	pugi::xml_attribute attr = node.attribute(attrName.c_str());
	if (attr) {
		return parseVec3(attr.as_string());
	}
	return defaultValue;
}

static inline std::vector<std::string> getStringList(const pugi::xml_node& node, std::string attrName, std::string defaultValue="") {
	std::string str = getString(node, attrName, defaultValue);

	std::vector<std::string> result;
	std::istringstream ss(str);
	std::string token;

	while (std::getline(ss, token, ',')) {
		token = trim(token); //Clean whitespace.
		if (!token.empty()) {
			result.push_back(token);
		}
	}
	return result;
}


}




//////// BODIES ////////

void getBodies(const pugi::xml_document& doc) {
	//Gets all celestial bodies (Including unnatural satellites too.)
	data::bodies.reserve(constants::NUMBER_OF_BODIES_TO_RESERVE); //Reserve space in the bodies dataset.
	pugi::xpath_node_set starNodes = doc.select_nodes("//bodies/star");


	if (starNodes.size() == 0u) {
		//No bodies to focus on.
		std::cout << "No bodies in the XML file - File must have at least 1 celestial body." << std::endl;
		return;
	}

	for (unsigned int starIndex=0u; starIndex<starNodes.size(); starIndex++) {
		//For every star in the file;
		pugi::xml_node starNode = starNodes[starIndex].node();
		std::string starSuffix = std::to_string(starIndex);
		glm::vec3 colour = xml::getVec3(starNode, "colour", glm::vec3(0.0f, 0.0f, 0.0f)) / 255.0f;
		utils::printVec3(colour);
		data::bodies.emplace_back(structs::CelestialBody(
			xml::getString(starNode, "name", "STAR_"+starSuffix),
			CT_STAR, //Type
			xml::getIVec2(starNode, "position", glm::ivec2(0, 0)),
			colour, //Colour of its orbital line.
			xml::getInt(starNode, "radius", 0.0f) * sim::SCALE_MULTIPLIER,
			0.0f, 0.0f, nullptr //No orbit, No parent.
		));
		structs::CelestialBody* star = &data::bodies.back();
		if constexpr (dev::SHOW_HEIRARCHY_CONSOLE) {std::cout << star->name << std::endl;}


		unsigned int planetIndex = 0u;
		auto planets = starNode.children("planet");
		star->children.reserve(std::distance(planets.begin(), planets.end()));
		for (pugi::xml_node planetNode : planets) {
			//For every planet orbiting this star;
			std::string planetSuffix = starSuffix + "_" + std::to_string(planetIndex);
			int planetOrbitalRadius = (xml::getFloat(planetNode, "orbitalRadius", 0.0f)*sim::SCALE_MULTIPLIER) + star->radius;
			glm::vec3 colour = xml::getVec3(planetNode, "colour", glm::vec3(0.0f, 0.0f, 0.0f)) / 255.0f;
			utils::printVec3(colour);
			data::bodies.emplace_back(structs::CelestialBody(
				xml::getString(planetNode, "name", "PLANET_"+planetSuffix),
				CT_PLANET, glm::ivec2(0, 0), //Type, start position (Gets overwritten when calculating orbit later)
				colour, //Colour of its orbital line.
				xml::getInt(planetNode, "radius", 0.0f) * sim::SCALE_MULTIPLIER,
				planetOrbitalRadius,  //Kilometres (km)
				xml::getFloat(planetNode, "orbitalPeriod", 0.0f) * sim::PERIOD_MULTIPLIER, //Days
				star //Parent star.
			));
			structs::CelestialBody* planet = &data::bodies.back();
			star->children.push_back(planet);
			if constexpr (dev::SHOW_HEIRARCHY_CONSOLE) {std::cout << " - " << planet->name << std::endl;}


			unsigned int satIndex = 0u;
			auto satellites = planetNode.children("satellite");
			planet->children.reserve(std::distance(satellites.begin(), satellites.end()));
			for (pugi::xml_node satNode : satellites) {
				//For every satellite orbiting this planet; [Moons, Stations.]
				std::string satSuffix = planetSuffix + "_" + std::to_string(satIndex);
				int satelliteOrbitalRadius = (xml::getFloat(satNode, "orbitalRadius", 0.0f)*sim::SCALE_MULTIPLIER) + planet->radius;
				glm::vec3 colour = xml::getVec3(satNode, "colour", glm::vec3(0.0f, 0.0f, 0.0f)) / 255.0f;
				utils::printVec3(colour);
				data::bodies.emplace_back(structs::CelestialBody(
					xml::getString(satNode, "name", "SATELLITE_"+satSuffix),
					CT_SATELLITE, glm::ivec2(0, 0), //Type, start position (Gets overwritten when calculating orbit later)
					colour, //Colour of its orbital line.
					xml::getInt(satNode, "radius", 0.0f) * sim::SCALE_MULTIPLIER,
					satelliteOrbitalRadius,  //Kilometres (km)
					xml::getFloat(satNode, "orbitalPeriod", 0.0f) * sim::PERIOD_MULTIPLIER, //Days
					planet //Parent planet.
				));
				structs::CelestialBody* satellite = &data::bodies.back();
				planet->children.push_back(satellite);
				satIndex++;
				if constexpr (dev::SHOW_HEIRARCHY_CONSOLE) {std::cout << "   = " << satellite->name << std::endl;}
			}

			planetIndex++;
		}

	}
	if constexpr (dev::SHOW_HEIRARCHY_CONSOLE) {std::cout << std::endl;}
}

//////// BODIES ////////





//////// ROUTES ////////

std::vector<structs::CelestialBody*> getLocations(std::vector<std::string>& locationNames) {
	std::vector<structs::CelestialBody*> result = {};
	result.reserve(locationNames.size());

	for (std::string lName : locationNames) {
		//Find first body with the same name (Case sensitive) and add a pointer to result.
		for (structs::CelestialBody& body : data::bodies) {
			if (
				(body.name == lName) && ((body.type == CT_PLANET) || (body.type == CT_SATELLITE)) //Only valid types to travel between.
			) {
				result.push_back(&body);
				break; //Match the first result.
			}
		}
	}
	return result;
}

void getRoutes(const pugi::xml_document& doc) {
	//Gets all routes.
	data::routes.reserve(constants::NUMBER_OF_ROUTES_TO_RESERVE); //Reserve space in the routes dataset.
	pugi::xpath_node_set routeNodes = doc.select_nodes("//routes/route");
	for (unsigned int routeIndex=0u; routeIndex<routeNodes.size(); routeIndex++) {
		pugi::xml_node routeNode = routeNodes[routeIndex].node();
		std::vector<std::string> locationNames = xml::getStringList(routeNode, "locations", "");
		if (locationNames.size() < 2) {continue; /* Empty or single-point routes ignored. */}
		data::routes.emplace_back(structs::Route(
			xml::getString(routeNode, "name", "INV-0000"),
			getLocations(locationNames)
		));
		if constexpr (dev::SHOW_ROUTES_CONSOLE) {
			structs::Route route = data::routes.back();
			std::cout << route.number << " : ";
			for (std::string lName : locationNames) {std::cout << lName << " → ";}
			std::cout << "..." << std::endl;
		}
	}
	if constexpr (dev::SHOW_ROUTES_CONSOLE) {std::cout << std::endl;}
}

//////// ROUTES ////////





//////// SHIPS ////////

bool getRoute(std::string& routeNumber, structs::Route** routePTR) {
	//Get the route from the routeNumber.
	for (structs::Route& route : data::routes) {
		if (route.number == routeNumber) {
			*routePTR = &route;
			return true; //Found a match.
		}
	}
	return false; //No match.
}

void getSShips(const pugi::xml_document& doc) {
	//Gets all ships.
	data::spacecraft.reserve(constants::NUMBER_OF_SSHIPS_TO_RESERVE); //Reserve space in the spacecraft dataset.
	pugi::xpath_node_set shipNodes = doc.select_nodes("//spacecraft/ship");

	for (unsigned int shipIndex=0u; shipIndex<shipNodes.size(); shipIndex++) {
		pugi::xml_node shipNode = shipNodes[shipIndex].node();
		std::string routeNumber = xml::getString(shipNode, "route", "");

		structs::Route* route = nullptr;
		bool validRoute = getRoute(routeNumber, &route);
		if (!validRoute) {continue; /* Ignore ships with invalid routes. */}
		structs::SpaceCraft ship = structs::SpaceCraft(
			xml::getString(shipNode, "name", "SHIP_"+std::to_string(shipIndex)),
			route
		);
		ship.journey = structs::Flight(
			ship.route->locations[0], ship.route->locations[1], routeNumber
		);

		data::spacecraft.push_back(ship);
		if constexpr (dev::SHOW_SHIPS_CONSOLE) {std::cout << ship.name << " : " << routeNumber << std::endl;}
	}
	if constexpr (dev::SHOW_SHIPS_CONSOLE) {std::cout << std::endl;}
}

//////// SHIPS ////////





//////// CAMERA ANGLES ////////

bool getFocussedBody(std::string name, structs::CelestialBody** bodyPTR) {
	//Find first body with the same name (Case sensitive) and return a pointer to it.
	for (structs::CelestialBody& body : data::bodies) {
		if (body.name == name) {
			*bodyPTR = &body;
			return true; //Match the first result.
		}
	}
	return false;
}

void getAngles(const pugi::xml_document& doc) {
	pugi::xpath_node_set viewNodes = doc.select_nodes("//camera/view");

	for (unsigned int viewIndex=0u; viewIndex<viewNodes.size(); viewIndex++) {
		pugi::xml_node viewNode = viewNodes[viewIndex].node();

		structs::CelestialBody* body = nullptr;
		std::string bodyName = xml::getString(viewNode, "body", "");
		bool success = getFocussedBody(bodyName, &body);
		if (!success) {continue; /* Invalid view, no body to focus with this name. */}

		data::views.push_back(structs::CameraView(
			xml::getString(viewNode, "name", "VIEW_" + std::to_string(viewIndex)), body,
			xml::getFloat(viewNode, "scale", 25u*body->radius),
			xml::getIVec2(viewNode, "offset", glm::ivec2(0, 0))
		));


		if constexpr (dev::SHOW_VIEWS_CONSOLE) {
			structs::CameraView view = data::views.back();
			std::cout << view.name << " : origin=" << bodyName << " : " << view.scale << "x : (" << view.offset.x << ", " << view.offset.y << ")" << std::endl;
		}
	}
	
	if (data::views.size() == 0u) {
		//No angles were provided in the file, or all were invalid. Assume the first celestial body is focussed.
		std::cout << "No camera views were specified in the data XML file. Defaulting to first celestial body, and 25x its radius." << std::endl;
		structs::CelestialBody* body = &(data::bodies[0]);
		data::views.push_back(structs::CameraView(
			"<FALLBACK_VIEW>", body, 25u*body->radius, glm::ivec2(0, 0)
		));
		if constexpr (dev::SHOW_VIEWS_CONSOLE) {
			structs::CameraView view = data::views.back();
			std::cout << view.name << " : origin=" << body->name << " : " << view.scale << "x : (" << view.offset.x << ", " << view.offset.y << ")" << std::endl;
		}
	}

	data::currentCameraViewIndex = 0u;
	data::view = &(data::views[0u]);

	if constexpr (dev::SHOW_VIEWS_CONSOLE) {std::cout << std::endl;}
}

//////// CAMERA ANGLES ////////





namespace loader {

void loadXMLdata(std::string& xmlFilePath) {
	pugi::xml_document doc;
	pugi::xml_parse_result parseResult = doc.load_file(xmlFilePath.c_str());

	if (!parseResult) {
		throw std::runtime_error("Failed to parse XML: " + std::string(parseResult.description()));
	}


	getBodies(doc);
	getRoutes(doc);
	getSShips(doc);
	getAngles(doc);


	//Calculate current state of the system;
	bodies::evaluate();
	spacecraft::evaluate();
}

}
