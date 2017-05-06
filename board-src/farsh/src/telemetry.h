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

	uint32_t checksumm;
} tel_t;

void send_packet();
void update_packet();

extern tel_t status;

#endif
