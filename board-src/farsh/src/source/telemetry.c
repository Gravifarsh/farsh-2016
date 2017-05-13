#include "all_includes.h"
#include "telemetry.h"
#include "sensors.h"
#include <rscs/crc.h>

static rscs_uart_bus_t * uart0;

int init_uart_radio()
{
	//uart0 = rscs_uart_init(RSCS_UART_ID_UART1, 	RSCS_UART_FLAG_ENABLE_TX |
	//											RSCS_UART_FLAG_BUFFER_TX |
	//											RSCS_UART_FLAG_ENABLE_RX |
	//											RSCS_UART_FLAG_BUFFER_RX);
	if (!uart0)
		return 1;

	rscs_uart_set_baudrate(uart0, 9600);
	rscs_uart_set_character_size(uart0, 8);
	rscs_uart_set_parity(uart0, RSCS_UART_PARITY_NONE);
	rscs_uart_set_stop_bits(uart0, RSCS_UART_STOP_BITS_ONE);

	return 0;
}

tel_t status = {
		.marker = 0xFCFC,
		.number = 0,

		.pressure_bmp = 0,
		.temperature_bmp = 0,
		.temperature_ds = 0,
		.lights = {0},
		.accelerations = {0},

		.checksumm = 0
};

void update_status()
{
	status.number++;
	//status.checksumm = rscs_crc8(&status,sizeof(status));
	//bmp_request();
	//ds_request();
	//adxl_request();
	ads_request();
}

void send_packet()
{
	rscs_uart_write(uart0,&status,sizeof(status));
}
