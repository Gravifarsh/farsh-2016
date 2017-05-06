#include "all_includes.h"
#include "sensors.h"
#include "telemetry.h"

rscs_uart_bus_t * uart0;

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


void hardware_init()
{
	config_radio_check();

	sei();
	init_uart_stdout();
	rscs_i2c_init();
	rscs_i2c_set_scl_rate(17);
	rscs_ow_init_bus();

	//rscs_servo_init(1);

	ads_init();
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
			update_packet();
			send_packet();
		}
	}
	if(false)
	{
		rscs_servo_timer_init();

		while(1)
		{
		}

	}

	while(1){}
	return 0;
}
