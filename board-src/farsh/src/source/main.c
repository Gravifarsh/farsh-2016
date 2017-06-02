#include "all_includes.h"
#include "sensors.h"
#include "status.h"
#include "servo.h"

void hardware_init()
{
	sei();

	//init_uart_radio();
	init_uart_stdout();

	//rscs_i2c_init();
	//rscs_i2c_set_scl_rate(100);
	//rscs_ow_init_bus();

	//rscs_servo_init(2);

	//ads_init();
	//bmp_init();
	//adxl_init();
	//ds_init();
}

int main()
{
	while(0){//CHECKING TO GO IN TECHNO MODE GOES HERE TODO
		comrade();
	}

	hardware_init();

	get_pressnlight(&status.check.press,&status.check.light,5);

	while(true)
	{
		update_status();
		send_packet();
		switch(status.mode){
			case MODE_STARTED:
				get_pressnlight(&status.check.press_t,&status.check.light_t,3);
				if(true){ //CONDITION GOES HERE TODO
					get_pressnlight(&status.check.press,&status.check.light,5);
					status.mode = MODE_IN_ROCKET;
				}
			break;

			case MODE_IN_ROCKET:
				get_pressnlight(&status.check.press_t,&status.check.light_t,3);
				if(true){ //CONDITION GOES HERE TODO
					status.mode = MODE_FLYING;
				}
			break;

			case MODE_FLYING:
				get_pressnlight(&status.check.press_t,&status.check.light_t,3);
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

