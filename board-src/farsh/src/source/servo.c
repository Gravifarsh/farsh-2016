#include "all_includes.h"
#include "sensors.h"
#include "servo.h"

#include <math.h>

void servo_oriantate()
{
	update_status(); // NOTE: Василий: Мне кажется обновлять глобальный статус должна на эта функция
					 // а какая-то более высокая сущность, принимающая решения
	int32_t res[8];
	double x=0, y=0;
	for(int i = 0; i < 8; i++){
		res[i] = status.ads[0].lights[i];//TODO
	}
	for(int i = 0; i < 8; i++)
	{
		x += res[i] * cos(i * 45 * M_PI / 180);
		y += res[i] * sin(i * 45 * M_PI / 180);
	}
	rscs_servo_set_angle(0,atan(y / x) * 180 / M_PI + 90);

}
