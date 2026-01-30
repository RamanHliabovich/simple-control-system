#include "drive.h"

drive::drive(std::string name, const pugi::xml_node drive_node)
	: name(name),
	drive_node(drive_node),
	run_outbit(nullptr),
	sts_inbit(nullptr)
{
}

drive::~drive()
{
	if (run_outbit)
	{
		delete run_outbit;
	}
	if (sts_inbit)
	{
		delete sts_inbit;
	}
}

int drive::init()
{
	// Parse run outbit
	pugi::xml_node run_outbit_node = drive_node.child("run_outbit");
	if (run_outbit_node)
	{
		std::string outbit_name = run_outbit_node.text().as_string();
		run_outbit = new outbit(outbit_name);
	}
	// Parse run inbit
	pugi::xml_node sts_inbit_node = drive_node.child("sts_inbit");
	if (sts_inbit_node)
	{
		std::string inbit_name = sts_inbit_node.text().as_string();
		sts_inbit = new inbit(inbit_name);
	}
	return 0;
}

int drive::start()
{
	run_outbit->write(1); // Set run_outbit to ON
	return 0;
}

int drive::stop()
{
	run_outbit->write(0); // Set run_outbit to OFF
	return 0;
}
