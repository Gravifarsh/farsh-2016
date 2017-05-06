#include "all_includes.h"
#include "telemetry.h"
#include "sensors.h"

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
	bmp_request();
	ds_request();
	adxl_request();
	ads_request();
}

void send_packet()
{

}
