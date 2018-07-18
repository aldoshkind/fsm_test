#pragma once

#include "movingobject.h"
#include "aila.h"
#include "fsm.h"

#define STATE_WAITING "waiting"
#define STATE_REACHING "reaching"
#define STATE_CONFIRMING "confirming"
#define STATE_MOVE_AWAY "move_away"

static const float alt_compare_thresh = 2.0;

class fsm_bombing : public fsm
{
public:
	/*constructor*/ fsm_bombing()
	{
		target = nullptr;
		aircraft = nullptr;
		bomb = nullptr;
		bombing_alt = 300;					// сравнительно безопасное значение высоты
	}

	virtual /*destructor*/ ~fsm_bombing() {}

	void update()
	{
		fsm::update();
	}

	void set_target(MovingObject *mo)
	{
		target = mo;
	}

	MovingObject *get_target() const
	{
		return target;
	}

	void set_aircraft(AILa *a)
	{
		aircraft = a;
	}

	AILa *get_aircraft() const
	{
		return aircraft;
	}

	float get_bombing_alt() const
	{
		return bombing_alt;
	}

	void set_bombing_alt(float alt)
	{
		bombing_alt = alt;
	}

	void set_bomb(MovingObject *b)
	{
		bomb = b;
	}

	MovingObject *get_bomb() const
	{
		return bomb;
	}

private:
	MovingObject	*target;
	AILa			*aircraft;
	MovingObject	*bomb;
	float			bombing_alt;
};

class state_waiting : public state
{
public:
	/*constructor*/ state_waiting(fsm_bombing *machine)
	{
		f = machine;
	}

	/*destructor*/ ~state_waiting(){}

	void update()
	{
		if(f == nullptr || f->get_aircraft() == nullptr)
		{
			return;
		}

		auto a = f->get_aircraft();
		a->ControlAltReq(a->GetPosition().z);
		a->ControlCourseReq(a->get_course());

		if(f->get_target() != nullptr)
		{
			f->set_state(STATE_MOVE_AWAY);
			return;
		}
	}

private:
	fsm_bombing *f;
};




class state_move_away : public state
{
public:
	/*constructor*/ state_move_away(fsm_bombing *machine)
	{
		f = machine;
	}

	/*destructor*/ ~state_move_away(){}

	void update()
	{
		if(f == nullptr || f->get_aircraft() == nullptr)
		{
			return;
		}

		if(f->get_target() == nullptr)
		{
			f->set_state(STATE_WAITING);
		}

		auto a = f->get_aircraft();

		float target_distance = a->getImpactShift(f->get_bombing_alt(), a->VectorVelocity().length2()) + a->GetTurnRadius(a->VectorVelocity().length2(), f->get_bombing_alt());
		float current_distance = (a->GetPosition() - f->get_target()->GetPosition()).length2();

		if(target_distance < current_distance)
		{
			f->set_state(STATE_REACHING);
		}
		else
		{
			a->ControlAltReq(f->get_bombing_alt());

			auto vec_out_of_target = a->GetPosition() - f->get_target()->GetPosition();
			float course = M_PI / 2.0 - atan2(vec_out_of_target.y, vec_out_of_target.x);

			a->ControlCourseReq(course);
		}
	}

private:
	fsm_bombing *f;
};



class state_reaching : public state
{
public:
	/*constructor*/ state_reaching(fsm_bombing *machine)
	{
		f = machine;
	}

	/*destructor*/ ~state_reaching(){}

	void update()
	{
		if(f == nullptr || f->get_aircraft() == nullptr)
		{
			return;
		}

		if(f->get_target() == nullptr)
		{
			f->set_state(STATE_WAITING);
			return;
		}

		auto a = f->get_aircraft();
		a->ControlAltReq(f->get_bombing_alt());
		a->ControlCourseReq(get_course_to_target());

		if(is_alt_achieved() && is_pos_achieved())
		{
			f->set_bomb(a->fireWeapon());
			f->set_state(STATE_CONFIRMING);
		}
	}

private:
	cVector get_target_pos_prediction(float prediction_gap = 0.0) const
	{
		auto a = f->get_aircraft();
		auto t = f->get_target();

		float fall_time = a->getFallTime(f->get_bombing_alt());
		auto target_pos_prediction = t->GetPosition() + t->VectorVelocity() * (fall_time + prediction_gap);

		return target_pos_prediction;
	}


	float get_course_to_target() const
	{
		auto a = f->get_aircraft();

		auto vec_to_target = get_target_pos_prediction() - a->GetPosition();

		float course = M_PI / 2.0 - atan2(vec_to_target.y, vec_to_target.x);

		return course;
	}

	bool is_alt_achieved() const
	{
		auto a = f->get_aircraft();
		return fabs(a->GetPosition().z - f->get_bombing_alt()) < alt_compare_thresh;
	}

	bool is_pos_achieved() const
	{
		auto a = f->get_aircraft();
		// текущее расстояние между точкой падения и предсказанным положением цели
		float current_distance = (a->getImpactPoint() - get_target_pos_prediction()).length2();

		// продолжительность такта
		float tick_size = 1.0f / f->get_target()->get_world()->get_update_frequency();
		// предсказанное положение ЛА
		cVector predicted_pos = a->GetPosition() + a->VectorVelocity() * tick_size;
		auto predicted_impact_point = a->getImpactPoint(predicted_pos, a->VectorVelocity());
		// предсказанное расстояние между точкой падения и предсказанным положением цели
		float predicted_distance = (predicted_impact_point - get_target_pos_prediction(tick_size)).length2();

		printf("bomb dist curr %f pred %f\n", current_distance, predicted_distance);

		// точка достигнута в том случае, если зашли в радиус поражения и предсказанное расстояние больше текущего, т.е. начнётся отдаление
		bool achieved = (current_distance < a->getImpactRadius()) && (predicted_distance > current_distance);

		return achieved;
	}

	fsm_bombing *f;
};



class state_confirming : public state
{
public:
	/*constructor*/ state_confirming(fsm_bombing *machine)
	{
		f = machine;
	}

	/*destructor*/ ~state_confirming(){}

	void update()
	{
		if(f == nullptr || f->get_aircraft() == nullptr)
		{
			return;
		}

		auto t = f->get_target();
		auto b = f->get_bomb();

		if(b == nullptr || t == nullptr)
		{
			f->set_state(STATE_WAITING);
		}

		auto a = f->get_aircraft();
		a->ControlAltReq(a->GetPosition().z);
		a->ControlCourseReq(a->get_course());

		if(b->IsDead() == false)
		{
			return;
		}

		if(t->IsDead())
		{
			f->set_state(STATE_WAITING);
			f->set_target(nullptr);
		}
		else
		{
			f->set_state(STATE_MOVE_AWAY);
		}
	}

private:
	fsm_bombing *f;
};
