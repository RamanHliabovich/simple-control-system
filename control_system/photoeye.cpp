#include <iostream>

#include "photoeye.h"

photoeye::photoeye(std::string name, const pugi::xml_node pe_node)
	: name(name),
	pe_node(pe_node),
	detect_inbit(nullptr)
{
}

photoeye::~photoeye()
{
	if (detect_inbit)
	{
		delete detect_inbit;
	}
}

int photoeye::init()
{
	// Parse detect inbit
	pugi::xml_node detect_inbit_node = pe_node.child("detect_inbit");
	if (detect_inbit_node)
	{
		std::string inbit_name = detect_inbit_node.text().as_string();
		detect_inbit = new inbit(inbit_name);
	}
	else
	{
		std::cout << "Photoeye [" << name << "] Error: No detect_inbit defined in configuration.\n";
		return -1;
	}

	return 0;
}

int photoeye::is_blocked()
{
	// Logic to check if the photoeye is blocked using detect_inbit
	return detect_inbit->read(); // Return 1 if blocked, 0 if not blocked
}
