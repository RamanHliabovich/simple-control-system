#include "cli_handler.h"
#include <sstream>
#include <algorithm>

cli_handler::cli_handler(conveyor_subsystem* subsystem)
	: subsystem(subsystem), should_exit(false)
{
	display_menu();
}

void cli_handler::display_menu()
{
	std::cout << "\n=== Conveyor Control System ===\n";
	std::cout << "start <conveyor_name>  - Start a conveyor\n";
	std::cout << "stop <conveyor_name>   - Stop a conveyor\n";
	std::cout << "start all              - Start all conveyors\n";
	std::cout << "stop all               - Stop all conveyors\n";
	std::cout << "list                   - List all conveyors\n";
	std::cout << "help                   - Show this menu\n";
	std::cout << "exit                   - Exit program\n";
	std::cout << "status <conveyor_name> - Check conveyor status\n";
	std::cout << "==============================\n\n";
}

void cli_handler::process_command(const std::string& command)
{
	if (command.empty())
		return;

	std::istringstream iss(command);
	std::string action, target;
	iss >> action >> target;

	// Convert to lowercase for case-insensitive comparison
	std::transform(action.begin(), action.end(), action.begin(), ::tolower);

	if (action == "exit")
	{
		std::cout << "Exiting...\n";
		should_exit = true;
	}
	else if (action == "help")
	{
		display_menu();
	}
	else if (action == "list")
	{
		list_conveyors();
	}
	else if (action == "start")
	{
		if (target.empty())
		{
			std::cout << "Error: Please specify conveyor name or 'all'\n";
		}
		else if (target == "all")
		{
			std::cout << "Starting all conveyors...\n";
			subsystem->start_all();
		}
		else
		{
			start_conveyor(target);
		}
	}
	else if (action == "stop")
	{
		if (target.empty())
		{
			std::cout << "Error: Please specify conveyor name or 'all'\n";
		}
		else if (target == "all")
		{
			std::cout << "Stopping all conveyors...\n";
			subsystem->stop_all();
		}
		else
		{
			stop_conveyor(target);
		}
	}
	else if (action == "status")
	{
		if (target.empty())
		{
			std::cout << "Error: Please specify conveyor name\n";
		}
		else
		{
			check_status(target);
		}
	}
	else
	{
		std::cout << "Unknown command: " << action << ". Type 'help' for available commands.\n";
	}
}

void cli_handler::start_conveyor(const std::string& name)
{
	if (subsystem->start_conveyor(name) == 0)
	{
		std::cout << "Conveyor [" << name << "] started.\n";
	}
	else
	{
		std::cout << "Error: Could not start conveyor [" << name << "]\n";
	}
}

void cli_handler::stop_conveyor(const std::string& name)
{
	if (subsystem->stop_conveyor(name) == 0)
	{
		std::cout << "Conveyor [" << name << "] stopped.\n";
	}
	else
	{
		std::cout << "Error: Could not stop conveyor [" << name << "]\n";
	}
}

void cli_handler::list_conveyors()
{
	subsystem->print_conveyors();
}

void cli_handler::check_status(const std::string& name)
{
	// You'll need to add a get_conveyor_status() method to conveyor_subsystem
	// For now, you can call print_conveyors() or add custom logic here
	std::string conv_sts = subsystem->get_conveyor_status(name);

	if (!conv_sts.empty())
	{
		std::cout << "Conveyor [" << name << "] Status: " << conv_sts << "\n";
	}
	else
	{
		std::cout << "Error: Could not retrieve status for conveyor [" << name << "]\n";
	}
}