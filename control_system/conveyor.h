#pragma once
#include <string>
#include <iostream>
#include <pugixml.hpp>

#include "drive.h"
#include "photoeye.h"

class conveyor
{
private:
	std::string name;
	pugi::xml_node conv_node;
	drive* mtr;
	photoeye* pe;

public:
	conveyor(std::string name, const pugi::xml_node conv_node);
	~conveyor();

	int init();

	int update();

	std::string get_name() { return name; };
};

