#include <stdio.h>
#include <math.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/eeprom.h>

#include <rscs/servo.h>
#include <rscs/ads1115.h>
#include <rscs/uart.h>
#include <rscs/stdext/stdio.h>

rscs_uart_bus_t * uart0;
// инициализация отладочного UART в STDOUT
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
	rscs_ads1115_start_continuous(*ads_one);
	rscs_ads1115_start_continuous(*ads_two);
}


uint8_t state EEMEM = 1;

int main()
{
	sei();
	init_uart_stdout();
	rscs_servo_init(3);
	rscs_servo_set_angle(0,0); //TODO Задать начальные углы сервам и откалибровать
	rscs_servo_set_angle(1,0);
	rscs_servo_set_angle(2,0);
	uint8_t st = eeprom_read_byte(&state);
	if(st == 0)
	{
		printf("FIRST\n");
		eeprom_write_byte(&state, 2);
		st = eeprom_read_byte(&state);
	}
	if(st == 1)
	{
		printf("SECOND\n");
	}
	if(st == 2)
	{

	}
	if(st == 3)
	{
		rscs_ads1115_t* ads_one;
		rscs_ads1115_t* ads_two;
		start_ads(&ads_one,&ads_two);
		while(1)
		{
			int16_t ResisVals[8];
			for(int i = 0; i < 4; i++)
			{
				rscs_ads1115_set_channel(ads_one, i + 4);
				rscs_ads1115_set_channel(ads_two, i + 4);
				rscs_ads1115_read(ads_one, ResisVals + i);
				rscs_ads1115_read(ads_two, ResisVals + i + 4);
			}
			float x = 0, y = 0;
			for(int i = 0; i < 8; i++)
			{
				x += ResisVals[i] * cos(i * 45);
				y += ResisVals[i] * sin(i * 45);
			}
			if(x == 0){rscs_servo_set_angle(0, 90);}
			else{rscs_servo_set_angle(0, atan(y/x));}
		}

	}

	while(1){}
	return 0;
}
