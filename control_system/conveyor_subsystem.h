#pragma once
#include <unordered_map>

#include "conveyor.h"
#include "pugixml.hpp"

class conveyor_subsystem
{
private:
	std::unordered_map<conveyor*, int> conveyors;
	pugi::xml_document doc;
	
public:
	conveyor_subsystem();
	~conveyor_subsystem();

	bool parse_xml(const std::string& filename);
	int init();
	int update();
};

