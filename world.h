#pragma once

#include <set>

#include "movingobject.h"

class world
{
public:
	std::set<MovingObject *> objects;

	void add_object(MovingObject *o)
	{
		objects.insert(o);
		o->set_world(this);
	}

	void update(float time)
	{
		for(const auto &o : objects)
		{
			if(o->IsDead() == false)
			{
				o->update(time);
			}
		}
	}
};
