#include <stdio.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/eeprom.h>

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




uint8_t state EEMEM = 1;

int main()
{
	sei();
	init_uart_stdout();
	uint8_t st = eeprom_read_byte(&state);
	if(st == 1)
	{
		printf("FIRST\n");
		eeprom_write_byte(&state, 2);
		st = eeprom_read_byte(&state);
	}
	if(st == 2)
	{
		printf("SECOND\n");
	}
	else
	{
		printf("ALARM!!11!!!");
	}

	while(1){}
	return 0;
}
