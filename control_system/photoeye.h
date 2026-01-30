#pragma once
#include <pugixml.hpp>
#include "inbit.h"

class photoeye
{
private:
	std::string name;
	pugi::xml_node pe_node;
	inbit* detect_inbit;

public:
	photoeye(std::string name, const pugi::xml_node pe_node);
	~photoeye();

	int init();
	int is_blocked();
};

