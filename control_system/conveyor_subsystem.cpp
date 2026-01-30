#include <iostream>
#include <stdexcept>

#include "conveyor_subsystem.h"

conveyor_subsystem::conveyor_subsystem()
	: conveyors()
{
}

conveyor_subsystem::~conveyor_subsystem()
{
	for (auto& pair : conveyors)
	{
		delete pair.first;
	}
}

bool conveyor_subsystem::parse_xml(const std::string& filename)
{
	try
	{
		pugi::xml_document& doc = this->doc; 
		pugi::xml_parse_result result = doc.load_file(filename.c_str());
		
		if (!result)
		{
			throw std::runtime_error("XML parse error: " + std::string(result.description()));
		}
		
		pugi::xml_node conveyors_node = doc.child("conveyors");
		if (!conveyors_node)
		{
			throw std::runtime_error("Missing 'conveyors' root element");
		}

		for (pugi::xml_node conveyor_node : conveyors_node.children("conveyor"))
		{
			conveyor* conv = new conveyor(conveyor_node.attribute("name").as_string(), conveyor_node);
			conveyors[conv] = conveyor_node.attribute("id").as_int();
		}

		return true;
	}
	catch (const std::exception& e)
	{
		std::cout << "Error: " << e.what() << "\n";
		return false;
	}
}

int conveyor_subsystem::init()
{
	for (auto& pair : conveyors)
	{
		conveyor* conv = pair.first;
		int id = pair.second;
		if (conv->init() != 0)
		{
			std::cout << "Conveyor Subsystem: Failed to initialize conveyor " << conv->get_name() << "]\n";
			return -1;
		}

		std::cout << "Conveyor Subsystem: Initialized conveyor [" << conv->get_name() << "]\n";
	}
	return 0;
}

int conveyor_subsystem::update()
{
	for (auto& pair : conveyors)
	{
		conveyor* conv = pair.first;
		int id = pair.second;
		conv->update();
	}

	return 0;
}
