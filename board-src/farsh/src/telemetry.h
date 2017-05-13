#ifndef DESC_H_
#define DESC_H_

#include "all_includes.h"

typedef struct {
	uint16_t marker;
	uint16_t number;

	int32_t pressure_bmp;
	int32_t temperature_bmp;
	int16_t temperature_ds;
	int16_t lights[8];
	int16_t accelerations[3];

	uint8_t checksumm;
} tel_t;

int init_uart_radio();
void send_packet();
void update_status();

extern tel_t status;

#endif
