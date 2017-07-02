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


void sd_setup_back()
{
	rscs_spi_set_clk(1000);
	rscs_spi_set_order(RSCS_SPI_ORDER_MSB_FIRST);
	rscs_spi_set_pol(RSCS_SPI_POL_SETUP_FALL_SAMPLE_RISE);
}

void sd_start()
{
	sd_desc = rscs_sd_init(&DDRB, &PORTB, (1 << 6));

	buf.carret = 0;
	buf.block = 0;

	status.err.sd = rscs_sd_startup(sd_desc);

	sd_setup_back();
}

void sd_write()
{
	if(512 - buf.carret > sizeof(packet))
	{
		for(int i = 0; i < sizeof(packet); i++)
		{
			buf.buffer[buf.carret++] = *((uint8_t*)&packet + i);
		}
	}
	else
	{
		int last = 512 - buf.carret;
		for(int i = 0; i < last; i++)
		{
			buf.buffer[buf.carret++] = *((uint8_t*)&packet + i);
		}

		rscs_sd_spi_setup();
		status.err.sd = rscs_sd_block_write(sd_desc, buf.block, &buf.buffer, 1);
		sd_setup_back();


		buf.block+=512;
		buf.carret = 0;

		if(!status.err.sd)
		{
			for(int i = last; i < sizeof(packet); i++)
			{
				buf.buffer[buf.carret++] = *((uint8_t*)&packet + i);
			}
		}
	}
}
