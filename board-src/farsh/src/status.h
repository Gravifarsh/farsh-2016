#ifndef DESC_H_
#define DESC_H_

#include "all_includes.h"

#define STAT_BUFF_S 5

typedef struct {
	uint16_t marker;
	uint16_t number;

	uint32_t press_b;
	int32_t temp_b;
	int16_t temp_ds;
	uint16_t power;
	int16_t lights[8];
	int16_t accelerations[3];
	uint8_t servo_pos[3];

	uint32_t e_bmp:3,e_ds:3,e_adxl:3,e_ads1:3,e_ads2:3,e_ina:3;

	uint8_t checksum;
} tel_t;

typedef struct{
	struct{
		uint32_t press;
		int16_t temp;
	}bmp[STAT_BUFF_S];

	struct{
		int16_t temp;
	}ds[STAT_BUFF_S];

	struct{
		int16_t x,y,z;
	}adxl[STAT_BUFF_S];

	struct{
		int16_t lights[8];
	}ads[STAT_BUFF_S];

	struct{
		uint16_t power;
	}ina[STAT_BUFF_S];

	struct{
		uint32_t press;
		uint16_t light;
		uint32_t press_t;
		uint16_t light_t;
		float p_k;
		float l_k;
	}check;

	struct{
		uint32_t ads1:3,ads2:3,bmp:3,adxl:3,ds:3,ina:3;
	}err;

	struct{
		uint8_t pos[3];
		bool opened;
	}servo;

	enum{
		MODE_STARTED,
		MODE_IN_ROCKET,
		MODE_FLYING,
		MODE_LANDED
	}mode;
}status_t;

int init_uart_radio();
int init_uart_stdout();

void send_packet();
void update_status();
void comrade();

extern status_t status;
extern tel_t packet;

#endif
