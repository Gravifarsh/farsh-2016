#include "all_includes.h"
#include "sensors.h"
#include "status.h"
#include "servo.h"

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

	//rscs_servo_timer_init();

	rscs_time_init();

	ina_init();
	ads_init();
	bmp_init();
	adxl_init();
	ds_init();
}

int main()
{
	hardware_init();

	while(1)
	{
		update_status();
		send_packet();
	}

	while(0){//CHECKING TO GO IN TECHNO MODE GOES HERE TODO
		comrade();
	}

	for(int i = 0; i < STAT_BUFF_S; i++){
		update_status();
	}

	get_pressnlight_normalized(&status.check.press,&status.check.light);

	uint32_t CheckingTime = status.time;

	while(true)
	{
		update_status();
		//send_packet();
		switch(status.mode){
			case MODE_STARTED:
				get_pressnlight_normalized(&status.check.press_t,&status.check.light_t);
				printf("%lu - %lu\n", status.time, CheckingTime);

				if((status.time - CheckingTime) > 3000){ //CONDITION GOES HERE TODO
					get_pressnlight_normalized(&status.check.press,&status.check.light);
					status.mode = MODE_IN_ROCKET;
					printf("IN ROCKET NOW!");
				}

				printf("%u %u\n",status.check.light_t, status.check.light);
				if(status.check.light_t >= status.check.light)
				{
					CheckingTime = status.time;
				}
			break;

			case MODE_IN_ROCKET:
				get_pressnlight_normalized(&status.check.press_t,&status.check.light_t);
				if(true){ //CONDITION GOES HERE TODO
					//FIRE-RAISER GOES HERE TODO
					status.mode = MODE_FLYING;
				}
			break;

			case MODE_FLYING:
				get_pressnlight_normalized(&status.check.press_t,&status.check.light_t);
				if(true){ //CONDITION GOES HERE TODO
					status.mode = MODE_LANDED;
				}
				break;
			case MODE_LANDED:
				servo_oriantate();
				break;
		}
	}
	return 0;
}

