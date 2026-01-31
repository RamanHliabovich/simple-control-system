#pragma once
#include <string>
class outbit
{
private:
	std::string name;
	int state; // 0 = off, 1 = on

public:
	outbit(const std::string& name)
		: name(name), state(0)
	{
	}

	~outbit()
	{
	}

	void write(int value) { state = value; }
	int read() const { return state; }
};

