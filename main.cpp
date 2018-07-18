#include <math.h>

#include <unistd.h>

#include "util.h"

#include "fsm_bombing.h"

int main()
{
	world w;

	AILa aircraft;
	// значения скоростей и положения выбраны для простоты отладки
	aircraft.GetPosition().x = 1000;
	aircraft.GetPosition().z = 500;
	aircraft.VectorVelocity().x = 50;
	aircraft.VectorVelocity().y = 0;
	MovingObject target;

	w.add_object(&aircraft);
	w.add_object(&target);

	fsm_bombing fb;
	state_waiting sw(&fb);
	state_reaching sr(&fb);
	state_confirming sc(&fb);
	state_move_away sm(&fb);

	fb.add_state(STATE_WAITING, &sw);
	fb.add_state(STATE_REACHING, &sr);
	fb.add_state(STATE_CONFIRMING, &sc);
	fb.add_state(STATE_MOVE_AWAY, &sm);

	fb.set_aircraft(&aircraft);

	fb.set_state(STATE_WAITING);

	// частота обновления - 10 Гц
	w.set_update_frequency(10);
	for(int i = 0 ; ; i += 1)
	{
		w.update(i / w.get_update_frequency());

		fb.update();
		if(i == 20)
		{
			fb.set_target(&target);
		}
		// частота обновления - 10 Гц, ждём 100 мс
		usleep(100000);
	}

	return 0;
}
