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
	for (unsigned int starIndex=0u; starIndex<starNodes.size(); starIndex++) {
		//For every star in the file;
		pugi::xml_node starNode = starNodes[starIndex].node();
		std::string starSuffix = std::to_string(starIndex);
		data::bodies.emplace_back(structs::CelestialBody(
			xml::getString(starNode, "name", "STAR_"+starSuffix),
			CT_STAR, //Type
			xml::getIVec2(starNode, "position", glm::ivec2(0, 0)),
			0.0f, 0.0f, nullptr //No orbit, No parent.
		));
		structs::CelestialBody* star = &data::bodies.back();
		if (dev::SHOW_HEIRARCHY_CONSOLE) {std::cout << star->name << std::endl;}


		unsigned int planetIndex = 0u;
		for (pugi::xml_node planetNode : starNode.children("planet")) {
			//For every planet orbiting this star;
			std::string planetSuffix = starSuffix + "_" + std::to_string(planetIndex);
			data::bodies.emplace_back(structs::CelestialBody(
				xml::getString(planetNode, "name", "PLANET_"+planetSuffix),
				CT_PLANET, glm::ivec2(0, 0), //Type, start position (Gets overwritten when calculating orbit later)
				xml::getFloat(planetNode, "orbitalRadius", 0.0f) * sim::SCALE_MULTIPLIER,  //Mega-Metres (1000km)
				xml::getFloat(planetNode, "orbitalPeriod", 0.0f) * sim::PERIOD_MULTIPLIER, //Days
				star //Parent star.
			));
			structs::CelestialBody* planet = &data::bodies.back();
			star->children.push_back(planet);
			if (dev::SHOW_HEIRARCHY_CONSOLE) {std::cout << " - " << planet->name << std::endl;}


			unsigned int satIndex = 0u;
			for (pugi::xml_node satNode : planetNode.children("satellite")) {
				//For every satellite orbiting this planet; [Moons, Stations.]
				std::string satSuffix = planetSuffix + "_" + std::to_string(satIndex);
				data::bodies.emplace_back(structs::CelestialBody(
					xml::getString(satNode, "name", "SATELLITE_"+satSuffix),
					CT_SATELLITE, glm::ivec2(0, 0), //Type, start position (Gets overwritten when calculating orbit later)
					xml::getFloat(satNode, "orbitalRadius", 0.0f) * sim::SCALE_MULTIPLIER,  //Mega-Metres (1000km)
					xml::getFloat(satNode, "orbitalPeriod", 0.0f) * sim::PERIOD_MULTIPLIER, //Days
					planet //Parent planet.
				));
				structs::CelestialBody* satellite = &data::bodies.back();
				planet->children.push_back(satellite);
				satIndex++;
				if (dev::SHOW_HEIRARCHY_CONSOLE) {std::cout << "   = " << satellite->name << std::endl;}
			}

			planetIndex++;
		}

	}
	if (dev::SHOW_HEIRARCHY_CONSOLE) {std::cout << std::endl;}
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
		if (dev::SHOW_ROUTES_CONSOLE) {
			structs::Route route = data::routes.back();
			std::cout << route.number << " : ";
			for (std::string lName : locationNames) {std::cout << lName << " → ";}
			std::cout << "..." << std::endl;
		}
	}
	if (dev::SHOW_ROUTES_CONSOLE) {std::cout << std::endl;}
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
		if (dev::SHOW_SHIPS_CONSOLE) {std::cout << ship.name << " : " << routeNumber << std::endl;}
	}
	if (dev::SHOW_SHIPS_CONSOLE) {std::cout << std::endl;}
}

//////// SHIPS ////////


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
}

}
