#include <assert.h>

#include "all_includes.h"
#include "status.h"
#include "sensors.h"
#include <rscs/crc.h>


static rscs_uart_bus_t * radio_uart;
static rscs_uart_bus_t *uart1;

status_t status = {
		.bmp.press = 0,
		.bmp.temp = 0,
		.ds.temp = 0,
		.ads.lights = {0},
		.adxl.x = 0,
		.adxl.y = 0,
		.adxl.z = 0,
};

tel_t packet = {
		.marker = 0xFAFA,
		.number = 0
};

int init_uart_stdout()
{
	uart1 = rscs_uart_init(RSCS_UART_ID_UART0, 	RSCS_UART_FLAG_ENABLE_TX |
												RSCS_UART_FLAG_BUFFER_TX |
												RSCS_UART_FLAG_ENABLE_RX |
												RSCS_UART_FLAG_BUFFER_RX);
	if (!uart1)
		return 1;

	rscs_uart_set_baudrate(uart1, 9600);
	rscs_uart_set_character_size(uart1, 8);
	rscs_uart_set_parity(uart1, RSCS_UART_PARITY_NONE);
	rscs_uart_set_stop_bits(uart1, RSCS_UART_STOP_BITS_ONE);

	stdout = rscs_make_uart_stream(uart1);

	return 0;
}

int init_uart_radio()
{
	radio_uart = rscs_uart_init(RSCS_UART_ID_UART1, 	RSCS_UART_FLAG_ENABLE_TX |
												RSCS_UART_FLAG_BUFFER_TX);
	if (!radio_uart)
		return 1;

	rscs_uart_set_baudrate(radio_uart, 9600);
	rscs_uart_set_character_size(radio_uart, 8);
	rscs_uart_set_parity(radio_uart, RSCS_UART_PARITY_NONE);
	rscs_uart_set_stop_bits(radio_uart, RSCS_UART_STOP_BITS_ONE);

	stdout = rscs_make_uart_stream(radio_uart);

	return 0;
}

void comrade()
{

}

void update_status()
{
	bmp_request();
	ds_request();
	adxl_request();
	ads_request();
}

void send_packet()
{
	packet.number++;
	packet.accelerations[0] = status.adxl.x;
	packet.accelerations[1] = status.adxl.y;
	packet.accelerations[2] = status.adxl.z;
	for(int i = 0; i < 8; i++){
		packet.lights[i] = status.ads.lights[i];
	}
	for(int i = 0; i < 3; i++){
		packet.servo_pos[i] = status.servo.pos[i];
	}
	packet.press_b = status.bmp.press;
	packet.temp_b = status.bmp.temp;
	packet.temp_ds = status.ds.temp;
	packet.power = status.ina.power;
	packet.checksum = rscs_crc8(&packet, sizeof(packet));
	rscs_uart_write(radio_uart,&status,sizeof(packet));
}
