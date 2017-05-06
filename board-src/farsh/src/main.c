#include <stdio.h>
#include <math.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include <rscs/onewire.h>
#include <rscs/servo.h>
#include <rscs/ads1115.h>
#include <rscs/uart.h>
#include <rscs/stdext/stdio.h>
#include <rscs/bmp280.h>
#include <rscs/ds18b20.h>
#include <rscs/adxl345.h>

#include <desc.h>

rscs_uart_bus_t * uart0;
rscs_bmp280_descriptor_t * bmp_desc;
rscs_adxl345_t * adxl_desc;
rscs_ds18b20_t * ds_desc;
ads_t ads;
tel_t packet;

void config_radio_check()
{
	DDRA |= (1<<1);
	PORTA |= (1<<1);

	/*DDRA &=~ (1<<0);
	PORTA |= (1<<0);

	if (0 == (PINA & (1<<0))){
		DDRA |= (1<<1);
		PORTA &=~ (1<<1);
		while(1){};
	}

	DDRA |= (1<<1);
	PORTA |= (1<<1);*/
}

int init_uart_stdout(void)
{
	uart0 = rscs_uart_init(RSCS_UART_ID_UART1, 	RSCS_UART_FLAG_ENABLE_TX |
												RSCS_UART_FLAG_BUFFER_TX |
												RSCS_UART_FLAG_ENABLE_RX |
												RSCS_UART_FLAG_BUFFER_RX);
	if (!uart0)
		return 1;

	rscs_uart_set_baudrate(uart0, 9600);
	rscs_uart_set_character_size(uart0, 8);
	rscs_uart_set_parity(uart0, RSCS_UART_PARITY_NONE);
	rscs_uart_set_stop_bits(uart0, RSCS_UART_STOP_BITS_ONE);

	stdout = rscs_make_uart_stream(uart0);
	return 0;
}


void ads_init(rscs_ads1115_t** ads_one, rscs_ads1115_t** ads_two)
{
	*ads_one = rscs_ads1115_init(RSCS_ADS1115_ADDR_GND);
	*ads_two = rscs_ads1115_init(RSCS_ADS1115_ADDR_VCC);
	rscs_ads1115_set_range(*ads_one, RSCS_ADS1115_RANGE_6DOT144);
	rscs_ads1115_set_range(*ads_two, RSCS_ADS1115_RANGE_6DOT144);
	rscs_ads1115_set_datarate(*ads_one, RSCS_ADS1115_DATARATE_32SPS);
	rscs_ads1115_set_datarate(*ads_two, RSCS_ADS1115_DATARATE_32SPS);
}


void bmp_init()
{
	bmp_desc = rscs_bmp280_initi2c(RSCS_BMP280_I2C_ADDR_HIGH);
	rscs_bmp280_parameters_t param;
	param.filter = RSCS_BMP280_FILTER_X16;
	param.pressure_oversampling = RSCS_BMP280_OVERSAMPLING_X16;
	param.temperature_oversampling = RSCS_BMP280_OVERSAMPLING_X16;
	param.standbytyme = RSCS_BMP280_STANDBYTIME_125MS;
	rscs_bmp280_setup(bmp_desc, &param);
	rscs_bmp280_changemode(bmp_desc,RSCS_BMP280_MODE_NORMAL);
}


void adxl_init()
{
	adxl_desc = rscs_adxl345_initi2c(RSCS_ADXL345_ADDR_MAIN);
	rscs_adxl345_set_rate(adxl_desc,RSCS_ADXL345_RATE_200HZ);
	rscs_adxl345_set_range(adxl_desc,RSCS_ADXL345_RANGE_2G);
}


void ds_init()
{
	ds_desc = rscs_ds18b20_init(0x00);
	rscs_ds18b20_start_conversion(ds_desc);
}


void bmp_request()
{
	int32_t rtemp,rpress,temp,press;
	rscs_bmp280_read(bmp_desc,&rpress,&rtemp);
	rscs_bmp280_calculate(rscs_bmp280_get_calibration_values(bmp_desc),rpress,rtemp,&press,&temp);
}


void ds_request()
{
	if(rscs_ds18b20_check_ready())
	{
		int16_t temp;
		rscs_ds18b20_read_temperature(ds_desc,&temp);
		rscs_ds18b20_count_temperature(ds_desc, temp);
		rscs_ds18b20_start_conversion(ds_desc);
	}
}


void adxl_request()
{
	int16_t x,y,z;
	rscs_adxl345_read(adxl_desc,&x,&y,&z);
}

void hardware_init()
{
	config_radio_check();

	sei();
	init_uart_stdout();
	rscs_i2c_init();
	rscs_i2c_set_scl_rate(17);
	rscs_ow_init_bus();

	rscs_servo_init(1);

	ads_init(&ads.one,&ads.two);
	bmp_init();
	adxl_init();
	ds_init();
}

int main()
{
	hardware_init();

	if(true)
	{
		while(1){

		}
	}
	if(false)
	{
		rscs_servo_timer_init();

		while(1)
		{
			int16_t ResisVals[4] = {0};
			double x = 0, y = 0;

			for(int i = 0; i < 4; i++)
			{
				rscs_ads1115_set_channel(ads.one, i + 4);
				while(ResisVals[i] <= 0){
						rscs_ads1115_read(ads.one, ResisVals + i);
				}

				x += (ResisVals[i] / 1000) * cos((float)(i) * 90 / 57.3);
				y += (ResisVals[i] / 1000) * sin((float)(i) * 90 / 57.3);
			}

			rscs_servo_set_angle(0,(int)(57.3 * atan(y/x)));
			_delay_ms(1000);
		}

	}

	while(1){}
	return 0;
}
