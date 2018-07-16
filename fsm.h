#pragma once

#include <stdio.h>

#include <algorithm>
#include <map>

class state
{
public:
	/*constructor*/ state(){}
	virtual /*destructor*/ ~state(){}

	virtual void update() = 0;
};

class fsm
{
public:
	/*constructor*/ fsm()
	{
		current_state = nullptr;
	}

	virtual /*destructor*/ ~fsm(){}

	void add_state(std::string name, state *st)
	{
		states[name] = st;
	}

	void set_state(std::string name)
	{
		if(states.find(name) == states.end())
		{
			return;
		}
		printf("state %s\n", name.c_str());
		current_state = states[name];
	}

	void update()
	{
		auto found = std::find_if(states.begin(), states.end(), [cs = current_state](const states_t::value_type &v){return (v.second == cs) ? true : false;});
		std::string state_name = (found == states.end()) ? "" : found->first;
		if(current_state == nullptr)
		{
			return;
		}
		current_state->update();
	}

protected:
	typedef std::map<std::string, state *>	states_t;

private:
	states_t								states;
	state *current_state;
};
