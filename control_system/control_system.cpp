#include "conveyor_subsystem.h"
#include "cli_handler.h"
#include "socket_server.h"

#include <chrono>
#include <thread>
#include <iostream>
#include <string>
#include <conio.h>

int main()
{
	conveyor_subsystem* c_subsystem = new conveyor_subsystem();

	//parse root XML
	if (!c_subsystem->parse_xml("config/conveyors.xml"))
	{
		std::cout << "Error: Failed to parse conveyor configuration XML.\n";
		return -1;
	}

	// Initialize subsystem
	if (c_subsystem->init() != 0)
	{
		std::cout << "Error: Failed to initialize conveyor subsystem.\n";
		return -1;
	}

	cli_handler cli(c_subsystem);

	// Start socket server for WPF communication
	socket_server server(c_subsystem, 5000);
	if (!server.start())
	{
		std::cout << "Error: Failed to start socket server.\n";
		return -1;
	}

	// Configuration
	const std::chrono::milliseconds CYCLE_TIME(50); // 50ms = 20 Hz update rate

	// Update loop
	auto last_time = std::chrono::high_resolution_clock::now();

	while (!cli.is_exit_requested())
	{
		// Check for user input (non-blocking) Windows only
		if (_kbhit())
		{
			std::string command;
			std::cout << "> ";
			std::getline(std::cin, command);
			cli.process_command(command);
		}

		int result = c_subsystem->update();

		// Check for shutdown signal or error
		if (result != 0)
		{
			break;
		}

		// Maintain consistent cycle time
		auto now = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time);

		if (elapsed < CYCLE_TIME)
		{
			std::this_thread::sleep_for(CYCLE_TIME - elapsed);
		}

		last_time = std::chrono::high_resolution_clock::now();
	}

	// Cleanup
	server.stop();  // Stop the socket server
	delete c_subsystem;
	return 0;
}