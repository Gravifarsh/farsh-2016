#include "all_includes.h"
#include "sensors.h"
#include "servo.h"

void servo_oriantate()
{
	update_status();
	int32_t res[8];
	double x=0, y=0;
	for(int i = 0; i < 8; i++){
		res[i] = status.ads.lights[i];
	}
	for(int i = 0; i < 8; i++)
	{
		x += res[i] * cos(i * 45 * 3.14159265359 / 180);
		y += res[i] * sin(i * 45 * 3.14159265359 / 180);
	}
	rscs_servo_set_angle(0,atan(y / x) * 180 / 3.14159265359 + 90);
}
