#ifndef REQUESTS_H_
#define REQUESTS_H_


#include "all_includes.h"

void bmp_request();
void ds_request();
void adxl_request();
void ads_request();

void ds_init();
void adxl_init();
void bmp_init();
void ads_init();

typedef struct{
	rscs_ads1115_t *one;
	rscs_ads1115_t *two;
} ads_t;

#endif
