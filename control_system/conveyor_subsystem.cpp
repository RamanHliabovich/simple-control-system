#include <iostream>
#include <stdexcept>

#include "conveyor_subsystem.h"

conveyor_subsystem::conveyor_subsystem()
	: conveyors(),
	uc(nullptr)
{
}

conveyor_subsystem::~conveyor_subsystem()
{
	for (auto& pair : conveyors)
	{
		delete pair.first;
	}

	if (uc)
	{
		delete uc;
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
	uc = new unity_client("127.0.0.1", 5001);

	if (!uc->connect())
	{
		std::cout << "Conveyor Subsystem: Failed to connect to Unity client.\n";
		return -1;
	}

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


//new functions from ai
int conveyor_subsystem::start_conveyor(const std::string& name)
{
	for (auto& pair : conveyors)
	{
		conveyor* conv = pair.first;
		if (conv->get_name() == name)
		{
			int result = conv->start();

			bool inbit = conv->is_running();

			if (uc && !uc->send_outbit_and_get_inbit(
				conv->get_name(),
				conv->get_drive_outbit_state(),
				inbit))
			{
				std::cout << "Conveyor Subsystem: Error communicating with Unity for conveyor [" << conv->get_name() << "]\n";
			}

			return result;
		}
	}
	return -1; // Conveyor not found
}

int conveyor_subsystem::stop_conveyor(const std::string& name)
{
	for (auto& pair : conveyors)
	{
		conveyor* conv = pair.first;
		if (conv->get_name() == name)
		{
			return conv->stop();
		}
	}
	return -1; // Conveyor not found
}

void conveyor_subsystem::start_all()
{
	for (auto& pair : conveyors)
	{
		conveyor* conv = pair.first;
		conv->start();
	}
}

void conveyor_subsystem::stop_all()
{
	for (auto& pair : conveyors)
	{
		conveyor* conv = pair.first;
		conv->stop();
	}
}

void conveyor_subsystem::print_conveyors()
{
	if (conveyors.empty())
	{
		std::cout << "No conveyors found.\n";
		return;
	}

	std::cout << "Active Conveyors:\n";
	for (auto& pair : conveyors)
	{
		conveyor* conv = pair.first;
		int id = pair.second;
		std::cout << "  - " << conv->get_name() << " (ID: " << id << ")\n";
	}
}

std::string conveyor_subsystem::get_conveyor_status(const std::string& name)
{
	for (auto& pair : conveyors)
	{
		conveyor* conv = pair.first;
		if (conv->get_name() == name)
		{
			int conv_run_sts = conv->is_running();
			// Placeholder for actual status retrieval logic
			return "Conveyor [" + name + "] is " + (conv_run_sts ? "RUNNING" : "STOPPED");
		}
	}
	return "Conveyor [" + name + "] not found.";
}
