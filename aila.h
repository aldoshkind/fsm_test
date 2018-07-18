#pragma once

#include <limits>

#include "movingobject.h"
#include "bomb.h"

class AILa : public MovingObject
{
public:
	AILa()
	{
		target_alt = std::numeric_limits<float>::quiet_NaN();
		target_course = std::numeric_limits<float>::quiet_NaN();
	}

	void ControlAltReq(float altReq) {target_alt = altReq;}					// поддержание заданной высоты
	void ControlCourseReq(float courseReq) {target_course = courseReq;}		// поддержание заданного курса
	float get_min_speed() const
	{
		return 100;															// умозрительно
	}
	float GetTurnRadius(float velocity, float /*altitude*/)	const			// возвращает радиус виража	для заданной высоты/скорости
	{
		velocity = std::max(velocity, get_min_speed());
		float len = velocity * 15;											// время разворота - 15 секунд
		float radius = len / M_PI;

		return radius;
	}

	float get_max_course_rate() const
	{
		cVector vel = VectorVelocity();
		float speed = std::max(vel.length2(), get_min_speed());
		return GetTurnRadius(speed, GetPosition().z) / speed;
	}

	float get_course() const
	{
		float course = M_PI / 2.0 - atan2(VectorVelocity().y, VectorVelocity().x);
		return course;
	}

	// для бомбы
	cVector getImpactPoint()												// "электронный выстрел" - возвращает точку	встречи бомбы с землёй
	{
		return getImpactPoint(GetPosition(), VectorVelocity());
	}

	cVector getImpactPoint(const cVector &pos, const cVector &velocity)
	{
		return pos + velocity * getFallTime(pos.z);
	}


	float getImpactShift(float alt, float speed)
	{
		return speed * getFallTime(alt);
	}
	float getFallTime(float alt) {return sqrt(2 * alt / 9.81);}							// получить время падения бомбы. Предполагаем вертикальная скорость = 0.
	float getImpactRadius() {return bomb::impact_radius;}
	bomb *fireWeapon()																	// выстрел оружием(сброс бомбы)
	{
		bomb *b = new bomb;
		b->GetPosition() = GetPosition();
		b->VectorVelocity() = VectorVelocity();
		b->set_fire_time(current_time);

		get_world()->add_object(b);

		return b;
	}

	void update(float time)
	{
		float dt = time - current_time;
		current_time = time;

		if(target_alt == target_alt)
		{
			float da = target_alt - GetPosition().z;
			float vspeed = da / dt;
			float max_vspeed = 50;														// максимальная скороподъёмность, умозрительно
			vspeed = clamped(vspeed, -max_vspeed, max_vspeed);
			GetPosition().z += vspeed * dt;
		}

		if(target_course == target_course)
		{
			float course = M_PI / 2.0 - atan2(VectorVelocity().y, VectorVelocity().x);
			float dc = target_course - course;
			dc = wrap<float>(dc, -M_PI, M_PI);
			float rspeed = dc / dt;
			float max_rspeed = get_max_course_rate();
			rspeed = clamped(rspeed, -max_rspeed, max_rspeed);
			rspeed *= dt;
			float cs = cos(rspeed);
			float sn = sin(rspeed);
			float rx = VectorVelocity().x * cs + VectorVelocity().y * sn;
			float ry = -VectorVelocity().x * sn + VectorVelocity().y * cs;
			VectorVelocity().x = rx;
			VectorVelocity().y = ry;
		}

		GetPosition().x += VectorVelocity().x * dt;
		GetPosition().y += VectorVelocity().y * dt;

		printf("aircraft pos is %f %f %f\n", GetPosition().x, GetPosition().y, GetPosition().z);
	}

private:
	float current_time;
	float fire_time;
	float fall_time;

	float target_alt;
	float target_course;
};
