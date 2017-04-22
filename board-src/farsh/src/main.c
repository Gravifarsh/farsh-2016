#include <stdio.h>
#include <math.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include <rscs/servo.h>
#include <rscs/ads1115.h>
#include <rscs/uart.h>
#include <rscs/stdext/stdio.h>
#include <rscs/bmp280.h>
#include <rscs/ds18b20.h>
#include <rscs/adxl345.h>

rscs_uart_bus_t * uart0;
rscs_bmp280_descriptor_t * bmp_desc;
rscs_adxl345_t * adxl_desc;
rscs_ds18b20_t * ds_desc;

int init_uart_stdout(void)
{
	uart0 = rscs_uart_init(RSCS_UART_ID_UART0, 	RSCS_UART_FLAG_ENABLE_TX |
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

inline void start_ads(rscs_ads1115_t** ads_one, rscs_ads1115_t** ads_two)
{
	*ads_one = rscs_ads1115_init(RSCS_ADS1115_ADDR_GND); //TODO Дописать адреса
	*ads_two = rscs_ads1115_init(RSCS_ADS1115_ADDR_VCC);
	rscs_ads1115_set_range(*ads_one, RSCS_ADS1115_RANGE_6DOT144); //TODO Посчитать диапозоны
	rscs_ads1115_set_range(*ads_two, RSCS_ADS1115_RANGE_6DOT144);
	rscs_ads1115_set_datarate(*ads_one, RSCS_ADS1115_DATARATE_32SPS);
	rscs_ads1115_set_datarate(*ads_two, RSCS_ADS1115_DATARATE_32SPS);
}

void bmp_init()
{
	bmp_desc = rscs_bmp280_initi2c(RSCS_BMP280_I2C_ADDR_LOW);//TODO  задать адрес
	rscs_bmp280_parameters_t param;
	param.filter = RSCS_BMP280_FILTER_X16;
	param.pressure_oversampling = RSCS_BMP280_OVERSAMPLING_X16;
	param.temperature_oversampling = RSCS_BMP280_OVERSAMPLING_X16;
	param.standbytyme = RSCS_BMP280_STANDBYTIME_125MS;
	printf("%d bmp setup\n",rscs_bmp280_setup(bmp_desc, &param));
	printf("%d bmp change\n",rscs_bmp280_changemode(bmp_desc,RSCS_BMP280_MODE_NORMAL));
}
void adxl_init()
{
	adxl_desc = rscs_adxl345_initi2c(RSCS_ADXL345_ADDR_ALT); //TODO задать адрес
	rscs_adxl345_set_rate(adxl_desc,RSCS_ADXL345_RATE_200HZ);
	rscs_adxl345_set_range(adxl_desc,RSCS_ADXL345_RANGE_2G);
}
void ds_init()
{
	ds_desc = rscs_ds18b20_init(0x00);
}

void bmp_request()
{
	int32_t rtemp,rpress,temp,press;
	printf("%d bmp read\n",rscs_bmp280_read(bmp_desc,&rpress,&rtemp));
	rscs_bmp280_calculate(rscs_bmp280_get_calibration_values(bmp_desc),rpress,rtemp,&press,&temp);
	printf("%ld.%ld temp %ldk press\n",temp/100,temp%100,press/1000);
}
void ds_request()
{
	for(int i = 0; i < 10; i++){
		if(rscs_ds18b20_check_ready()){
			int16_t temp2;
			printf("%d ds read",rscs_ds18b20_read_temperature(ds_desc,&temp2));
			printf("%d temp 2\n",temp2);
			return;
		}
		_delay_ms(50);
	}
	printf("DS BUSY\n");
}
void adxl_request()
{
	int16_t x,y,z;
	printf("%d adxl read\n",rscs_adxl345_read(adxl_desc,&x,&y,&z));
	printf("x %d; y %d; z %d\n",x,y,z);
}

//uint8_t state EEMEM = 2;

int main()
{
	sei();
	init_uart_stdout();

	rscs_i2c_init();
	rscs_i2c_set_scl_rate(100);

	//bmp_init();
	//adxl_init();
	//ds_init();

	//uint8_t st = eeprom_read_byte(&state);
	//printf("st %d\n",st);

	printf("INITIALIZED\n");

	if(false)
	{
		while(1){bmp_request();}
	}
	if(true)
	{
		rscs_ads1115_t* ads_one;
		ads_one = rscs_ads1115_init(RSCS_ADS1115_ADDR_VCC);
		printf("%d\n",rscs_ads1115_set_range(ads_one, RSCS_ADS1115_RANGE_6DOT144));
		printf("%d\n",rscs_ads1115_set_datarate(ads_one, RSCS_ADS1115_DATARATE_32SPS));
		printf("STARTED \n");

		while(1)
		{
			int16_t ResisVals[4];
			for(int i = 0; i < 1; i++)
			{
				printf("%d ", rscs_ads1115_set_channel(ads_one, i + 4));
				printf("%d ", rscs_ads1115_wait_result(ads_one));
				printf("%d ", rscs_ads1115_read(ads_one, ResisVals + i));
				printf("%d; ", rscs_ads1115_start_single(ads_one));
				printf("%d : %d \n", i, ResisVals[i]);
				_delay_ms(100);
			}
			//float x = 0, y = 0;
			/*for(int i = 0; i < 4; i++)
			{
				x += ResisVals[i] * cos(i * 45);
				y += ResisVals[i] * sin(i * 45);
			}*/
		}

	}

	while(1){}
	return 0;
}
