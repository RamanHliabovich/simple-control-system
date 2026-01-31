#pragma once

#include <pugixml.hpp>

#include "outbit.h"
#include "inbit.h"

class drive
{
private:
	std::string name;
	pugi::xml_node drive_node;
	outbit* run_outbit;
	inbit* sts_inbit;

public:
	drive(std::string name, const pugi::xml_node drive_node);
	~drive();
	
	int init();
	int start();
	int stop();

	int is_running();

};

