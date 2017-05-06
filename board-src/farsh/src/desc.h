#ifndef DESC_H_
#define DESC_H_

#include <rscs/ads1115.h>

typedef struct {
	rscs_ads1115_t * one;
	rscs_ads1115_t * two;
} ads_t;

typedef struct {
	uint16_t marker;
	uint16_t number;

	int32_t pressure_bmp;
	int32_t temperature_bmp;
	int16_t temperature_ds;


	uint32_t checksumm;
} tel_t;

#endif /* DESC_H_ */
