#pragma once

#include <stdio.h>

#include "movingobject.h"
#include "world.h"

class bomb : public MovingObject
{
public:
	static const int impact_radius = 50;

	void update(float time)
	{
		float dt = time - previous_time;
		previous_time = time;

		VectorVelocity().z = VectorVelocity().z - 9.81 * dt;
		GetPosition() = GetPosition() + VectorVelocity() * dt;

		if(GetPosition().z < 0.0)
		{
			set_dead(true);

			world *w = get_world();
			if(w == nullptr)
			{
				return;
			}
			printf("explosion\n");
			for(auto &o : w->objects)
			{
				if(o->GetPosition().z > 0)
				{
					continue;
				}
				if(o == this)
				{
					continue;
				}
				if((o->GetPosition() - GetPosition()).length2() < impact_radius)
				{
					printf("object hit\n");
					o->set_dead(true);
				}
			}
		}
	}

	void set_fire_time(float t)
	{
		previous_time = t;
	}

private:
	float previous_time;
};
