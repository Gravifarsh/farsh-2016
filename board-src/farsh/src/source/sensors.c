#include "all_includes.h"
#include "sensors.h"
#include "status.h"

rscs_bmp280_descriptor_t * bmp_desc = NULL;
rscs_adxl345_t * adxl_desc = NULL;
rscs_ds18b20_t * ds_desc = NULL;
rscs_ina219_t * ina_desc = NULL;
ads_t ads={
		.one = NULL,
		.two = NULL
};

//ЗАПИСЬ ОШИБКИ И ВЫХОД ИЗ ФУНКЦИИ ЗА НЕ ИМЕНИЕМ СМЫСЛА ДАЛЬШЕ РАБОТАТЬ
#define WADDUP(OP, err) err = OP; if(err != RSCS_E_NONE) return;
//ПРОБУЕМ ИНИЦИАЛИЗИРОВАТЬ ЕЩЁ РАЗ, ЕСЛИ НЕ ПОЛУЧИЛОСЬ
#define CHECKNTRY(func , err) if(err) {err = 0; func(); if(err) {return;}}

//ДА! СЛЕДУЮЩИЕ ТРИ ФУНКЦИИ ТАК И ЗАДУМЫВАЛИСЬ! ОЧЕНЬ ХИТРЫЙ МАКРОС!
void _ads1_init()
{
	WADDUP(rscs_ads1115_set_range(ads.one, RSCS_ADS1115_RANGE_6DOT144), status.err.ads1);
	WADDUP(rscs_ads1115_set_datarate(ads.one, RSCS_ADS1115_DATARATE_32SPS), status.err.ads2);
}
void _ads2_init()
{
	WADDUP(rscs_ads1115_set_range(ads.two, RSCS_ADS1115_RANGE_6DOT144), status.err.ads1);
	WADDUP(rscs_ads1115_set_datarate(ads.two, RSCS_ADS1115_DATARATE_32SPS), status.err.ads2);
}

void ads_init()
{
	if(!ads.one){ads.one = rscs_ads1115_init(RSCS_ADS1115_ADDR_GND);} // А ВДРУГ УЖЕ ИНИЦИАЛИЗИРОВАЛЛИ?
	if(!ads.two){ads.two = rscs_ads1115_init(RSCS_ADS1115_ADDR_VCC);}

	_ads1_init();
	_ads2_init();
}

void ina_init()
{
	if(!ina_desc){ina_desc = rscs_ina219_init(0x40);}// А ВДРУГ УЖЕ ИНИЦИАЛИЗИРОВАЛЛИ?

	rscs_ina219_set_cal(ina_desc, 4096);
	uint16_t temp = 0;
	temp |= (1<<INA_BADC2) | (1<<INA_BADC1)
				| (1<<INA_SADC2) | (1<<INA_SADC1)
				| (1<<PG0) | (1<<PG1);
	rscs_ina219_start_continuous(ina_desc,RSCS_INA219_CHANNEL_SHUNT);
}

void bmp_init()
{
	if(!bmp_desc){bmp_desc = rscs_bmp280_initspi(&PORTB, &DDRB, 5);}   // А ВДРУГ УЖЕ ИНИЦИАЛИЗИРОВАЛЛИ?

	rscs_bmp280_parameters_t param;
	param.filter = RSCS_BMP280_FILTER_X16;
	param.pressure_oversampling = RSCS_BMP280_OVERSAMPLING_X16;
	param.temperature_oversampling = RSCS_BMP280_OVERSAMPLING_X16;
	param.standbytyme = RSCS_BMP280_STANDBYTIME_125MS;

	WADDUP(rscs_bmp280_setup(bmp_desc, &param), status.err.bmp);
	WADDUP(rscs_bmp280_changemode(bmp_desc,RSCS_BMP280_MODE_NORMAL), status.err.bmp);
}

void adxl_init()
{
	if(!adxl_desc){adxl_desc = rscs_adxl345_initspi(4);}// А ВДРУГ УЖЕ ИНИЦИАЛИЗИРОВАЛЛИ? TODO

	WADDUP(rscs_adxl345_startup(adxl_desc), status.err.adxl);
	rscs_adxl345_set_rate(adxl_desc, RSCS_ADXL345_RATE_200HZ);
	rscs_adxl345_set_range(adxl_desc, RSCS_ADXL345_RANGE_2G);
}

void ds_init()
{
	if(!ds_desc){ds_desc = rscs_ds18b20_init(0x00);}// А ВДРУГ УЖЕ ИНИЦИАЛИЗИРОВАЛЛИ?

	WADDUP(rscs_ds18b20_start_conversion(ds_desc), status.err.ds);
}

void ina_request()
{
	CHECKNTRY(ina_init, status.err.ina);

	for(int i = STAT_BUFF_S - 1; i > 0; i--)
	{
		status.ina[i - 1] = status.ina[i];
	}

	rscs_ina219_read(ina_desc, RSCS_INA219_CHANNEL_SHUNT, &status.ina[0].power);

	/*uint16_t temp;
	printf("\n============================\n");

	printf("%d", rscs_ina219_start_single(ina_desc, RSCS_INA219_CHANNEL_BUS));
	printf("%d\n", rscs_ina219_wait_single(ina_desc));
	printf("%d 0x00:", rscs_ina_read_reg(ina_desc, 0x00, &temp));
	printf(" %u\n", temp);
	printf("%d 0x01:", rscs_ina_read_reg(ina_desc, 0x01, &temp));
	printf(" %u\n", temp);
	printf("%d 0x02:", rscs_ina_read_reg(ina_desc, 0x02, &temp));
	printf(" %u\n", temp);
	printf("%d 0x03:", rscs_ina_read_reg(ina_desc, 0x03, &temp));
	printf(" %u\n", temp);
	printf("%d 0x04:", rscs_ina_read_reg(ina_desc, 0x04, &temp));
	printf(" %u\n", temp);
	printf("%d 0x05:", rscs_ina_read_reg(ina_desc, 0x05, &temp));
	printf(" %u\n", temp);
	printf("%d", rscs_ina219_read(ina_desc, RSCS_INA219_CHANNEL_BUS, &temp));
	printf(" %u;\n ", temp);
	printf("--------------------------\n");

	printf("%d", rscs_ina219_start_single(ina_desc, RSCS_INA219_CHANNEL_SHUNT));
	printf("%d\n", rscs_ina219_wait_single(ina_desc));
	printf("%d 0x00:", rscs_ina_read_reg(ina_desc, 0x00, &temp));
	printf(" %u\n", temp);
	printf("%d 0x01:", rscs_ina_read_reg(ina_desc, 0x01, &temp));
	printf(" %u\n", temp);
	printf("%d 0x02:", rscs_ina_read_reg(ina_desc, 0x02, &temp));
	printf(" %u\n", temp);
	printf("%d 0x03:", rscs_ina_read_reg(ina_desc, 0x03, &temp));
	printf(" %u\n", temp);
	printf("%d 0x04:", rscs_ina_read_reg(ina_desc, 0x04, &temp));
	printf(" %u\n", temp);
	printf("%d 0x05:", rscs_ina_read_reg(ina_desc, 0x05, &temp));
	printf(" %u\n", temp);
	printf("%d", rscs_ina219_read(ina_desc, RSCS_INA219_CHANNEL_SHUNT, &temp));
	printf(" %u; \n", temp);
	printf("--------------------------\n");

	printf("%d", rscs_ina219_start_single(ina_desc, RSCS_INA219_CHANNEL_POWER));
	printf("%d\n", rscs_ina219_wait_single(ina_desc));
	printf("%d 0x00:", rscs_ina_read_reg(ina_desc, 0x00, &temp));
	printf(" %u\n", temp);
	printf("%d 0x01:", rscs_ina_read_reg(ina_desc, 0x01, &temp));
	printf(" %u\n", temp);
	printf("%d 0x02:", rscs_ina_read_reg(ina_desc, 0x02, &temp));
	printf(" %u\n", temp);
	printf("%d 0x03:", rscs_ina_read_reg(ina_desc, 0x03, &temp));
	printf(" %u\n", temp);
	printf("%d 0x04:", rscs_ina_read_reg(ina_desc, 0x04, &temp));
	printf(" %u\n", temp);
	printf("%d 0x05:", rscs_ina_read_reg(ina_desc, 0x05, &temp));
	printf(" %u\n", temp);
	printf("%d", rscs_ina219_read(ina_desc, RSCS_INA219_CHANNEL_POWER, &temp));
	printf(" %u;\n ", temp);
	printf("--------------------------\n");*/
}

void bmp_request()
{
	CHECKNTRY(bmp_init, status.err.bmp);

	for(int i = STAT_BUFF_S - 1; i > 0; i--)
	{
		status.bmp[i - 1] = status.bmp[i];
	}

	int32_t rtemp,rpress,temp,press;
	rscs_bmp280_read(bmp_desc,&rpress,&rtemp);
	rscs_bmp280_calculate(rscs_bmp280_get_calibration_values(bmp_desc),rpress,rtemp,&press,&temp);
	status.bmp[0].press = press;
	status.bmp[0].temp = temp;
}

void ds_request()
{
	CHECKNTRY(ds_init, status.err.ds);

	if(rscs_ds18b20_check_ready())
	{
		for(int i = STAT_BUFF_S - 1; i > 0; i--)
		{
			status.ds[i - 1] = status.ds[i];
		}

		int16_t temp;
		rscs_ds18b20_read_temperature(ds_desc,&temp);
		status.ds[0].temp = (int16_t)(rscs_ds18b20_count_temperature(ds_desc, temp) * 100);
		rscs_ds18b20_start_conversion(ds_desc);
	}
}

void adxl_request()
{
	CHECKNTRY(adxl_init, status.err.adxl);

	for(int i = STAT_BUFF_S - 1; i > 0; i--)
	{
		status.adxl[i - 1] = status.adxl[i];
	}

	int16_t x,y,z;
	rscs_adxl345_read(adxl_desc,&x,&y,&z);
	status.adxl[0].x = x;
	status.adxl[0].y = y;
	status.adxl[0].z = z;
}

void ads_request()
{
	CHECKNTRY(_ads1_init, status.err.ads1);
	CHECKNTRY(_ads2_init, status.err.ads2);

	for(int i = STAT_BUFF_S - 1; i > 0; i--)
	{
		status.ads[i - 1] = status.ads[i];
	}

	for(int i = 0; i < 4; i++){
		rscs_ads1115_take(ads.one, i + 4, status.ads[0].lights + i);//TODO
		rscs_ads1115_take(ads.two, i + 4, status.ads[0].lights + i + 4);
	}
}

void get_pressnlight_normalized(uint32_t* press,uint16_t* light)
{
	uint64_t p = 0,l = 0;
	for(int i = 0; i < STAT_BUFF_S; i++)
	{
		p+=status.bmp[i].press;
		for(int j = 0; j < 8; j++)
		{
			l += status.ads[i].lights[j];
		}
	}
	p /= STAT_BUFF_S;
	l /= STAT_BUFF_S;
	*press = p;
	*light = l;
}
