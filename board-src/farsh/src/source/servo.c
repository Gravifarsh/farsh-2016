#include "all_includes.h"
#include "sensors.h"
#include "servo.h"

#define DELTA 30

int min(int a, int b)
{
	if(a > b) return b;
	return a;
}

int abs(int a)
{
	if(a > 0) return a;
	return -a;
}

void servo_oriantate()
{
	ads_request();
	int32_t res[8];
	double x=0, y=0;

	res[0] = status.ads.lights[3];
	res[1] = status.ads.lights[2];
	res[2] = status.ads.lights[5];
	res[3] = status.ads.lights[7];
	res[4] = status.ads.lights[4];
	res[5] = status.ads.lights[6];
	res[6] = status.ads.lights[1];
	res[7] = status.ads.lights[0];

	for(int i = 0; i < 8; i++)
	{
		x += res[i] * cos(i * 45 * M_PI / 180);
		y += res[i] * sin(i * 45 * M_PI / 180);
	}

	status.servo.pos[0] = atan(y / x) * 180 / 3.14159265359 + 90;
	rscs_servo_set_angle(0,status.servo.pos[0]);

	int delta;
	while(1)
	{
		delta = min(180 - status.servo.pos[1], DELTA);

		ina_request();
		rscs_servo_set_angle(1,status.servo.pos[1] + delta);
		rscs_servo_set_angle(2,status.servo.pos[2] - delta);
		_delay_ms(100);

		ina_request();
		if((abs(status.ina[0].power) <= abs(status.ina[1].power)) ||( delta < DELTA))
		{
			rscs_servo_set_angle(1,status.servo.pos[1]);
			rscs_servo_set_angle(2,status.servo.pos[2]);

			while(1)
			{
				delta = min(status.servo.pos[1], DELTA);
				ina_request();

				rscs_servo_set_angle(1,status.servo.pos[1] - delta);
				rscs_servo_set_angle(2,status.servo.pos[2] + delta);
				_delay_ms(100);

				ina_request();
				if((abs(status.ina[0].power) <= abs(status.ina[1].power)) ||( delta < DELTA))
				{
					rscs_servo_set_angle(1,status.servo.pos[1]);
					rscs_servo_set_angle(2,status.servo.pos[2]);
					break;
				}
				else
				{
					status.servo.pos[1] -= delta;
					status.servo.pos[2] += delta;
				}
			}
			break;
		}
		else
		{
			status.servo.pos[1] += delta;
			status.servo.pos[2] -= delta;
		}
	}
}

void servo_scan()
{

}
