/*
 * sd.c
 *
 *  Created on: 29 июня 2017 г.
 *      Author: developer
 */
#include "sd.h"


typedef struct{
	uint8_t buffer[512];
	uint16_t carret;
	uint16_t block;
}buffer_t;

buffer_t buf;

rscs_sdcard_t* sd_desc;

rscs_e sd_start()
{
	sd_desc = rscs_sd_init(&DDRB, &PORTB, (1 << 6));

	buf.carret = 0;
	buf.block = 0;

	return rscs_sd_startup(sd_desc);
}

rscs_e sd_write()
{
	if(512 - buf.carret > sizeof(packet))
	{
		for(int i = 0; i < sizeof(packet); i++)
		{
			buf.buffer[buf.carret++] = *((uint8_t*)&packet + i);
		}

		return RSCS_E_NONE;
	}
	else
	{
		int last = 512 - buf.carret;
		for(int i = 0; i < last; i++)
		{
			buf.buffer[buf.carret++] = *((uint8_t*)&packet + i);
		}

		rscs_e er = rscs_sd_block_write(sd_desc, buf.block, &buf.buffer, 1);

		if(er) return er;

		buf.block++;
		buf.carret = 0;

		for(int i = last; i < sizeof(packet); i++)
		{
			buf.buffer[buf.carret++] = *((uint8_t*)&packet + i);
		}

		return RSCS_E_NONE;
	}
}
