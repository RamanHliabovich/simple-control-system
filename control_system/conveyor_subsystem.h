#pragma once
#include <unordered_map>

#include "conveyor.h"
#include "pugixml.hpp"
#include "unity_client.h"

class conveyor_subsystem
{
private:
	std::unordered_map<conveyor*, int> conveyors;
	pugi::xml_document doc;

	unity_client* uc;
	
public:
	conveyor_subsystem();
	~conveyor_subsystem();

	bool parse_xml(const std::string& filename);
	int init();
	int update();

	int start_conveyor(const std::string& name);
	int stop_conveyor(const std::string& name);
	void start_all();
	void stop_all();
	void print_conveyors();
	std::string get_conveyor_status(const std::string& name);
};

