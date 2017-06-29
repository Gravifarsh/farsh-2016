#include "all_includes.h"
#include "sensors.h"
#include "status.h"
#include "servo.h"
#include "sd.h"

void hardware_init()
{

	sei();

	//init_uart_radio();
	init_uart_stdout();

	rscs_spi_init();
	rscs_spi_set_pol(RSCS_SPI_POL_SETUP_FALL_SAMPLE_RISE);
	rscs_i2c_init();
	rscs_i2c_set_scl_rate(100);
	rscs_ow_init_bus();

	rscs_servo_init(3);
	rscs_servo_calibrate(0, 1.8, 2.4);
	rscs_servo_calibrate(1, 1.05, 1.85);
	rscs_servo_calibrate(2, 1.05, 1.85);

	rscs_servo_set_angle(0, 180);
	rscs_servo_set_angle(1, 90);
	rscs_servo_set_angle(2,90);
	status.servo.pos[1] = status.servo.pos[2] = status.servo.pos[0] = 90;

	rscs_servo_timer_init();

	rscs_time_init();

	sd_start();

	ina_init();
	ads_init();
	bmp_init();
	adxl_init();
	ds_init();
}

void fire_raiser(){}

int main()
{
	hardware_init();

	while(0){//CHECKING TO GO IN TECHNO MODE GOES HERE TODO
		comrade();
	}

	uint16_t Light,CurLight;

	//get_light(&Light, 10);

	uint32_t CheckingTime = 0;

	while(true)
	{
		update_status();
		update_packet(PACKET_STANDART);
		send_packet();
		sd_write();

		printf("%d %d %d\n", status.adxl.x, status.adxl.y, status.adxl.z);

		switch(status.mode){
			case MODE_STARTED:{
				if(status.time >= 900000){
					status.mode = MODE_IN_ROCKET;
				}
			}
			break;

			case MODE_IN_ROCKET:{
				if(!CheckingTime){CheckingTime = status.time;}

				get_light(&CurLight, 3); //idk какое n

				if(status.time - CheckingTime > 5000)
				{
					status.mode = MODE_FLYING;
					fire_raiser();
				}

				if(CurLight <= Light * 0.8)
				{
					CheckingTime = status.time;
				}
			}
			break;

			case MODE_FLYING:{
				if(get_bar_dheight() < 2)
				{
					status.mode = MODE_LANDED;
					rscs_servo_set_angle(0,90);//TODO какой там угол?
				}
			}
			break;

			case MODE_LANDED:{
				servo_oriantate();
				servo_scan(); //TODO Написать функцию
			}
			break;
		}

		if(status.time > 2400000 && status.mode != MODE_LANDED)
		{
			rscs_servo_set_angle(0,90);
			status.mode = MODE_LANDED;
		}
	}
	return 0;
}

