#pragma once
#include <string>
class inbit
{
private:
	std::string name;
	int state; // 0 = off, 1 = on
public:
	inbit(const std::string& name)
		: name(name), state(0)
	{
	}

	~inbit()
	{
	}

	int read()  { return state;}
};

