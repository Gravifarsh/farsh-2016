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
	//rscs_i2c_init();
	//rscs_i2c_set_scl_rate(100);
	//rscs_ow_init_bus();

	//rscs_servo_init(2);

	//ina_init();
	//ads_init();
	bmp_init();
	//adxl_init();
	//ds_init();
}

int main()
{
	while(0){//CHECKING TO GO IN TECHNO MODE GOES HERE TODO
		comrade();
	}

	hardware_init();

	while(1)
	{
		update_status();
		printf("bmp: press - %ld; temp - %d;\n", status.bmp[0].press, status.bmp[0].temp);
		//printf("adxl: x - %d; y - %d; z - %d;\n", status.adxl[0].x, status.adxl[0].y, status.adxl[0].z);
	}






	for(int i = 0; i < STAT_BUFF_S; i++){
		update_status();
	}

	get_pressnlight_normalized(&status.check.press,&status.check.light);

	while(true)
	{
		update_status();
		send_packet();
		switch(status.mode){
			case MODE_STARTED:
				get_pressnlight_normalized(&status.check.press_t,&status.check.light_t);
				if(true){ //CONDITION GOES HERE TODO
					get_pressnlight_normalized(&status.check.press,&status.check.light);
					status.mode = MODE_IN_ROCKET;
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

