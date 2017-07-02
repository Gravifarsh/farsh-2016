#include "all_includes.h"
#include "sensors.h"
#include "status.h"
#include "servo.h"
#include "sd.h"

void hardware_init()
{
	sei();

	rscs_time_init();

	//init_uart_radio();
	init_uart_stdout();

	rscs_spi_init();
	rscs_spi_set_clk(1000);
	rscs_spi_set_order(RSCS_SPI_ORDER_MSB_FIRST);
	rscs_spi_set_pol(RSCS_SPI_POL_SETUP_FALL_SAMPLE_RISE);

	rscs_i2c_init();
	rscs_i2c_set_scl_rate(100);
	rscs_ow_init_bus();

	rscs_servo_init(3);
	rscs_servo_calibrate(0, 1.8, 2.4);
	rscs_servo_calibrate(1, 0.6, 2.4);
	rscs_servo_calibrate(2, 0.6, 2.4);

	rscs_servo_set_angle(0, 90);
	rscs_servo_set_angle(1, 90);
	rscs_servo_set_angle(2,90);
	status.servo.pos[1] = status.servo.pos[2] = status.servo.pos[0] = 90;

	ina_init();
	ads_init();
	bmp_init();
	adxl_init();
	ds_init();

	sd_start();

	DDRA |= (1<<3);
}

void fire_raiser(){}

int main()
{
	hardware_init();

	while(0){//CHECKING TO GO IN TECHNO MODE GOES HERE TODO
		comrade();
	}

	uint16_t Light,CurLight;

	get_light(&Light, 10);

	uint32_t CheckingCycles = 0;

	while(true)
	{
		update_status();
		update_packet(PACKET_STANDART);
		send_packet();
		sd_write();

		switch(status.mode){
			case MODE_STARTED:{
				if(status.time >= 30000){
					printf("IN ROCKET\n");
					status.mode = MODE_IN_ROCKET;
				}
			}
			break;

			case MODE_IN_ROCKET:{
				get_light(&CurLight, 3); //idk какое n

				if(CheckingCycles > 3)
				{
					printf("FLYING\n");
					status.mode = MODE_FLYING;
					fire_raiser();
				}

				if(CurLight <= Light * 0.9)
				{
					CheckingCycles = 0;
				}
				else
				{
					CheckingCycles++;
				}
			}
			break;

			case MODE_FLYING:{
				if(get_bar_dheight() < 2)
				{
					CheckingCycles++;
				}
				else
				{
					CheckingCycles = 0;
				}
				if(CheckingCycles > 5)
				{
					printf("LANDED\n");
					status.mode = MODE_LANDED;
					rscs_servo_timer_init();
					rscs_servo_set_angle(0,30);
				}
			}
			break;

			case MODE_LANDED:{
				servo_oriantate();
				if((uint32_t)(status.time / 60000) % 5 ==0) servo_scan();
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

