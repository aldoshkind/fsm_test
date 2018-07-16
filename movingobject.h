#pragma once

#include "cvector.h"

class world;

class MovingObject
{
public:
	/*constructor*/ MovingObject()
	{
		dead = false;
		w = nullptr;
	}

	virtual /*destructor*/ ~MovingObject()
	{
		//
	}

	cVector &GetPosition(){return position;}				// возвращает позицию объекта
	cVector GetPosition()const{return position;}			// возвращает позицию объекта
	cVector &VectorVelocity(){return velocity;}				// возвращает векторную скорость
	cVector VectorVelocity()const{return velocity;}			// возвращает векторную скорость
	bool IsDead() const {return dead;}						// возвращает состояние жизни объекта
	void set_dead(bool dead){this->dead = dead;}

	world *get_world() const
	{
		return w;
	}

	void set_world(world *w)
	{
		this->w = w;
	}

	virtual void update(float time) {}

private:
	cVector position;
	cVector velocity;
	bool dead;
	world *w;
};
