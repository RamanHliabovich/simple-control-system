#include "conveyor.h"

conveyor::conveyor(std::string name,const pugi::xml_node conv_node)
	: name(name),
	conv_node(conv_node),
	mtr(nullptr), 
	pe(nullptr)
{
}

conveyor::~conveyor()
{
	if (mtr)
	{
		delete mtr;
	}
	if (pe)
	{
		delete pe;
	}
}

int conveyor::init()
{
	// Initialize drive
	pugi::xml_node drive_node = conv_node.child("drive");
	if (drive_node)
	{
		std::string drive_name = drive_node.attribute("name").as_string();
		mtr = new drive(drive_name, drive_node);

		if (!mtr)
		{
			std::cout << "Conveyor [" << name << "] Error: Failed to create drive [" << drive_name << "]\n";
			return -1;
		}

		if (mtr->init() != 0)
		{
			std::cout << "Conveyor [" << name << "] Error: Failed to initialize drive [" << drive_name << "]\n";
			return -1;
		}

	}
	else
	{
		std::cout << "Conveyor [" << name << "] Error: No drive defined in configuration.\n";
		return -1;
	}

	// Initialize photoeye
	pugi::xml_node photoeye_node = conv_node.child("photoeye");
	if (photoeye_node)
	{
		std::string photoeye_name = photoeye_node.attribute("name").as_string();
		pe = new photoeye(photoeye_name, photoeye_node);

		if (!pe)
		{
			std::cout << "Conveyor [" << name << "] Error: Failed to create photoeye [" << photoeye_name << "]\n";
			return -1;
		}

		if (pe->init() != 0)
		{
			std::cout << "Conveyor [" << name << "] Error: Failed to initialize photoeye [" << photoeye_name << "]\n";
			return -1;
		}
	}
	else
	{
		std::cout << "Conveyor [" << name << "] Warning: No photoeye defined in configuration.\n";
	}

	return 0;
}

int conveyor::update()
{
	//std::cout << "Conveyor [" << name << "] Updating.\n";
	return 0;
}

int conveyor::start()
{
	if (mtr)
	{
		return mtr->start();
	}
	return -1;
}

int conveyor::stop()
{
	if (mtr)
	{
		return mtr->stop();
	}
	return -1;
}

int conveyor::is_running()
{
	// Placeholder implementation
	return mtr->is_running(); // 1 if running, 0 if stopped
}