#pragma once

#include <string>
#include <iostream>
#include "conveyor_subsystem.h"

class cli_handler
{
private:
	conveyor_subsystem* subsystem;
	bool should_exit;

	void display_menu();
	void start_conveyor(const std::string& name);
	void stop_conveyor(const std::string& name);
	void check_status(const std::string& name);  // NEW METHOD
	void list_conveyors();

public:
	cli_handler(conveyor_subsystem* subsystem);
	void process_command(const std::string& command);
	bool is_exit_requested() const { return should_exit; }
	void reset_exit_flag() { should_exit = false; }
};