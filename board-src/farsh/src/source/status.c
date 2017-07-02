#include <assert.h>

#include "all_includes.h"
#include "status.h"
#include "sensors.h"
#include <rscs/crc.h>


static rscs_uart_bus_t * radio_uart;
static rscs_uart_bus_t *uart1;

status_t status = {
		.err.bmp = 0,
		.err.ds = 0,
		.err.ads1 = 0,
		.err.ads2 = 0,
		.err.adxl = 0,
		.err.ina = 0,
		.err.sd = 0
};

tel_t packet = {
		.marker = 0xFAFCFAFD,
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

	stdin = stdout = rscs_make_uart_stream(uart1);


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

void update_status()
{
	ina_request();
	bmp_request();
	ds_request();
	adxl_request();
	ads_request();
	status.time = rscs_time_get();
}

void update_packet(packet_mode mode)
{
	packet.number++;

	switch(mode){
		case PACKET_STANDART:
			packet.accelerations[0] = status.adxl.x;
			packet.accelerations[1] = status.adxl.y;
			packet.accelerations[2] = status.adxl.z;
			for(int i = 0; i < 8; i++){
				packet.lights[i] = status.ads.lights[i];
			}
			for(int i = 0; i < 3; i++){
				packet.servo_pos[i] = status.servo.pos[i];
			}
			packet.press_b = status.bmp[0].press;
			packet.temp_b = status.bmp[0].temp;
			packet.temp_ds = status.ds.temp;
			packet.power_ina = status.ina[0].power;
			packet.time = status.time;

			packet.e_ads1 = status.err.ads1;
			packet.e_ads2 = status.err.ads2;
			packet.e_adxl = status.err.adxl;
			packet.e_bmp = status.err.bmp;
			packet.e_ds = status.err.ds;
			packet.e_ina = status.err.ina;
			packet.e_sd = status.err.sd;

			break;
		case PACKET_SCAN_SERVO:
			packet.accelerations[0] = 0;
			packet.accelerations[1] = 0;
			packet.accelerations[2] = 0;
			for(int i = 0; i < 8; i++){
				packet.lights[i] = 0;
			}
			for(int i = 0; i < 3; i++){
				packet.servo_pos[i] = status.servo.pos[i];
			}
			packet.press_b = 0;
			packet.temp_b = 0;
			packet.temp_ds = 0;
			packet.power_ina = status.ina[0].power;
			packet.time = 0;

			packet.e_ads1 = 0;
			packet.e_ads2 = 0;
			packet.e_adxl = 0;
			packet.e_bmp = 0;
			packet.e_ds = 0;
			packet.e_ina = 0;
			packet.e_sd = 0;

			break;
	}
	packet.checksum = rscs_crc8(&packet, sizeof(packet) - sizeof(packet.checksum));
}

void send_packet()
{
	//rscs_uart_write(radio_uart,&packet,sizeof(packet));
	rscs_uart_write(uart1,&packet,sizeof(packet));
}
