#include <all_includes.h>
#include "sensors.h"
#include "telemetry.h"

rscs_bmp280_descriptor_t * bmp_desc;
rscs_adxl345_t * adxl_desc;
rscs_ds18b20_t * ds_desc;
ads_t ads;


void ads_init()
{
	ads.one = rscs_ads1115_init(RSCS_ADS1115_ADDR_GND);
	ads.two = rscs_ads1115_init(RSCS_ADS1115_ADDR_VCC);
	rscs_ads1115_set_range(ads.one, RSCS_ADS1115_RANGE_6DOT144);
	rscs_ads1115_set_range(ads.two, RSCS_ADS1115_RANGE_6DOT144);
	rscs_ads1115_set_datarate(ads.one, RSCS_ADS1115_DATARATE_32SPS);
	rscs_ads1115_set_datarate(ads.two, RSCS_ADS1115_DATARATE_32SPS);
}


void bmp_init()
{
	bmp_desc = rscs_bmp280_initi2c(RSCS_BMP280_I2C_ADDR_HIGH);
	rscs_bmp280_parameters_t param;
	param.filter = RSCS_BMP280_FILTER_X16;
	param.pressure_oversampling = RSCS_BMP280_OVERSAMPLING_X16;
	param.temperature_oversampling = RSCS_BMP280_OVERSAMPLING_X16;
	param.standbytyme = RSCS_BMP280_STANDBYTIME_125MS;
	rscs_bmp280_setup(bmp_desc, &param);
	rscs_bmp280_changemode(bmp_desc,RSCS_BMP280_MODE_NORMAL);
}


void adxl_init()
{
	adxl_desc = rscs_adxl345_initi2c(RSCS_ADXL345_ADDR_MAIN);
	rscs_adxl345_set_rate(adxl_desc,RSCS_ADXL345_RATE_200HZ);
	rscs_adxl345_set_range(adxl_desc,RSCS_ADXL345_RANGE_2G);
}


void ds_init()
{
	ds_desc = rscs_ds18b20_init(0x00);
	rscs_ds18b20_start_conversion(ds_desc);
}

void bmp_request()
{
	int32_t rtemp,rpress,temp,press;
	rscs_bmp280_read(bmp_desc,&rpress,&rtemp);
	rscs_bmp280_calculate(rscs_bmp280_get_calibration_values(bmp_desc),rpress,rtemp,&press,&temp);
	status.pressure_bmp = press;
	status.temperature_bmp = temp;
}

void ds_request()
{
	if(rscs_ds18b20_check_ready())
	{
		int16_t temp;
		rscs_ds18b20_read_temperature(ds_desc,&temp);
		rscs_ds18b20_count_temperature(ds_desc, temp);
		status.temperature_ds = temp;
		rscs_ds18b20_start_conversion(ds_desc);
	}
}

void adxl_request()
{
	int16_t x,y,z;
	rscs_adxl345_read(adxl_desc,&x,&y,&z);
	status.accelerations[0] = x;
	status.accelerations[1] = y;
	status.accelerations[2] = z;
}

void ads_request()
{

}

