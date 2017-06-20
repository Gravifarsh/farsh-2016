
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <util/delay.h>
#include <rscs/spi.h>
#include "../../librscs/rscs/error.h"
#include "../adxl345.h"

/* Команды на чтение и запись */
#define RSCS_ADXL345_SPI_READ        (1 << 7)	//бит на чтение
#define RSCS_ADXL345_SPI_WRITE       (0 << 7)	//бит на запись
#define RSCS_ADXL345_SPI_MB          (1 << 6)	//бит чтение/запись нескольких байт (в противном случае только одного байта)

/* ADXL345 Адреса регистров */
#define RSCS_ADXL345_DEVID			0x00 // R	Device ID
#define RSCS_ADXL345_OFSX            0x1E // R/W X-axis offset.
#define RSCS_ADXL345_OFSY            0x1F // R/W Y-axis offset.
#define RSCS_ADXL345_OFSZ            0x20 // R/W Z-axis offset.
#define RSCS_ADXL345_BW_RATE         0x2C // R/W Data rate and power mode control.
#define RSCS_ADXL345_POWER_CTL       0x2D // R/W Power saving features control.
#define RSCS_ADXL345_INT_ENABLE      0x2E // R/W Interrupt enable control.
#define RSCS_ADXL345_INT_MAP         0x2F // R/W Interrupt mapping control.
#define RSCS_ADXL345_INT_SOURCE      0x30 // R   Source of interrupts.
#define RSCS_ADXL345_DATA_FORMAT     0x31 // R/W Data format control.
#define RSCS_ADXL345_DATAX0          0x32 // R   X-Axis Data 0.
#define RSCS_ADXL345_DATAX1          0x33 // R   X-Axis Data 1.
#define RSCS_ADXL345_DATAY0          0x34 // R   Y-Axis Data 0.
#define RSCS_ADXL345_DATAY1          0x35 // R   Y-Axis Data 1.
#define RSCS_ADXL345_DATAZ0          0x36 // R   Z-Axis Data 0.
#define RSCS_ADXL345_DATAZ1          0x37 // R   Z-Axis Data 1.
#define RSCS_ADXL345_FIFO_CTL        0x38 // R/W FIFO control.
#define RSCS_ADXL345_FIFO_STATUS     0x39 // R   FIFO status.

/* ADXL345_POWER_CTL Определение регистра */
#define RSCS_ADXL345_PCTL_LINK       (1 << 5)
#define RSCS_ADXL345_PCTL_AUTO_SLEEP (1 << 4)
#define RSCS_ADXL345_PCTL_MEASURE    (1 << 3)
#define RSCS_ADXL345_PCTL_SLEEP      (1 << 2)
#define RSCS_ADXL345_PCTL_WAKEUP(x)  ((x) & 0x3)

/* ADXL345_INT_ENABLE / ADXL345_INT_MAP / ADXL345_INT_SOURCE Определение регистра */
#define RSCS_ADXL345_DATA_READY      (1 << 7)
#define RSCS_ADXL345_SINGLE_TAP      (1 << 6)
#define RSCS_ADXL345_DOUBLE_TAP      (1 << 5)
#define RSCS_ADXL345_ACTIVITY        (1 << 4)
#define RSCS_ADXL345_INACTIVITY      (1 << 3)
#define RSCS_ADXL345_FREE_FALL       (1 << 2)
#define RSCS_ADXL345_WATERMARK       (1 << 1)
#define RSCS_ADXL345_OVERRUN         (1 << 0)

/* ADXL345_DATA_FORMAT Определение регистра */
#define RSCS_ADXL345_SELF_TEST       (1 << 7)
#define RSCS_ADXL345_SPI_BIT             (1 << 6)
#define RSCS_ADXL345_INT_INVERT      (1 << 5)
#define RSCS_ADXL345_FULL_RES        (1 << 3)
#define RSCS_ADXL345_JUSTIFY         (1 << 2)
#define RSCS_ADXL345_RANGE(x)        ((x) & 0x3)

/* ADXL345_BW_RATE Определение регистра */
#define RSCS_ADXL345_LOW_POWER       (1 << 4)
#define RSCS_ADXL345_RATE(x)         ((x) & 0xF)


/* ADXL345 Full Resolution Scale Factor */
#define RSCS_ADXL345_SCALE_FACTOR    		0.0039
#define RSCS_ADXL345_OFFSET_SCALE_FACTOR		15.6

// описание стурктуры дескриптора
struct rscs_adxl345_t
{
	rscs_adxl345_range_t range;
	uint8_t pin_n;
};


/*******СЛУЖЕБНЫЕ ФУНКЦИИ*******/

#define SSPI_PORT PORTB
#define SSPI_DDR DDRB
#define SSPI_PIN PINB
#define SSPI_MISO (3)
#define SSPI_MOSI (2)
#define SSPI_CLK (1)
#define SSPI_CS (0)

//#define RSCS_SPI_PORTX	(PORTB)
//#define RSCS_SPI_DDRX	(DDRB)
//#define RSCS_SPI_MISO	(3)
//#define RSCS_SPI_MOSI	(2)
//#define RSCS_SPI_SCK	(1)
//#define RSCS_SPI_SS		(0)


rscs_adxl345_t* rscs_adxl345_initspi(uint8_t pin_n)
{
	rscs_adxl345_t* retval = (rscs_adxl345_t*)malloc(sizeof(rscs_adxl345_t));

	if(!retval) return NULL;

	retval->pin_n = pin_n;

	SSPI_DDR |= (1<<retval->pin_n);
	SSPI_PORT &= ~(1<<retval->pin_n);
	_delay_ms(10);
	SSPI_PORT |= (1<<retval->pin_n);

	return retval;
}

void rscs_adxl345_getRegisterValue(rscs_adxl345_t * device, uint8_t registerAddress,
		void * buffer_, size_t buffer_size)
{
	uint8_t * buffer = (uint8_t *)buffer_;
	if(buffer_size - 1){registerAddress |= (3<<6);}
	else{registerAddress |= (1<<7);}

	SSPI_PORT &= ~(1<<device->pin_n);

	rscs_spi_write(&registerAddress,1);
	rscs_spi_read(buffer, buffer_size, 0xFF);

	SSPI_PORT |= (1<<device->pin_n);
}

void rscs_adxl345_setRegisterValue(rscs_adxl345_t * device, uint8_t registerAddress, uint8_t registerValue)
{
	registerAddress &= ~(1<<7);

	SSPI_PORT &= ~(1<<device->pin_n);

	rscs_spi_write(&registerAddress, 1);
	rscs_spi_write(&registerValue, 1);

	SSPI_PORT |= (1<<device->pin_n);
}

rscs_e rscs_adxl345_startup(rscs_adxl345_t * adxl) {
	uint8_t devid = 0;
	rscs_adxl345_getRegisterValue(adxl, 0x00, &devid,1);

	if(devid !=  229)  {
		return RSCS_E_INVRESP;
	}

	//смещение по осям XYZ равно 0 (по умолчанию)
	rscs_adxl345_setRegisterValue(adxl, RSCS_ADXL345_OFSX, 0);
	rscs_adxl345_setRegisterValue(adxl, RSCS_ADXL345_OFSY, 0);
	rscs_adxl345_setRegisterValue(adxl, RSCS_ADXL345_OFSZ, 0);
	//LOW_POWER off, 100Гц (по умолчанию)
	rscs_adxl345_setRegisterValue(adxl, RSCS_ADXL345_BW_RATE, RSCS_ADXL345_RATE_100HZ);

	rscs_adxl345_setRegisterValue(adxl, RSCS_ADXL345_DATA_FORMAT,
			adxl->range			// диапазон
			| RSCS_ADXL345_FULL_RES	// FULL_RES = 1 (для всех диапазонов использовать максимальное разрешение 4 mg/lsb)
	);
	//переводит акселерометр из режима ожидания в режим измерения
	rscs_adxl345_setRegisterValue(adxl, RSCS_ADXL345_POWER_CTL,	RSCS_ADXL345_PCTL_MEASURE);
	return RSCS_E_NONE;
}


void rscs_adxl345_deinit(rscs_adxl345_t * device)
{
	free(device);
}


/* УСТАНОВКА ПРЕДЕЛОВ ИЗМЕРЕНИЙ*/
void rscs_adxl345_set_range(rscs_adxl345_t * device, rscs_adxl345_range_t range)
{
	uint8_t data = 0;

	rscs_adxl345_getRegisterValue(device, RSCS_ADXL345_DATA_FORMAT, &data, 1);
	data &= ~( (1 << 1) | 1 );			//очищаем 2 младших бита регистра BW_RATE
	data |= RSCS_ADXL345_RANGE(range);	//и записываем новое значение
	rscs_adxl345_setRegisterValue(device, RSCS_ADXL345_DATA_FORMAT, data);

	device->range = range;
}


/* УСТАНОВКА ЧАСТОТЫ ИЗМЕРЕНИЙ*/
void rscs_adxl345_set_rate(rscs_adxl345_t * device, rscs_adxl345_rate_t rate)
{
	uint8_t data = 0;

	rscs_adxl345_getRegisterValue(device, RSCS_ADXL345_BW_RATE, &data, 1);
	data &= ~(0xF);						//очищаем 4 младших бита регистра BW_RATE
	data |= RSCS_ADXL345_RATE(rate);	//и записываем новое значение
	rscs_adxl345_setRegisterValue(device, RSCS_ADXL345_BW_RATE, data);
}


/* УСТАНОВКА СМЕЩЕНИЯ РЕЗУЛЬТАТОВ ПО ОСЯМ X, Y, Z*/
void rscs_adxl345_set_offset(rscs_adxl345_t * device, float mg_x, float mg_y, float mg_z)
{
	int8_t ofs_x;
	int8_t ofs_y;
	int8_t ofs_z;

	ofs_x = (int8_t) round(mg_x / RSCS_ADXL345_OFFSET_SCALE_FACTOR);
	ofs_y = (int8_t) round(mg_y / RSCS_ADXL345_OFFSET_SCALE_FACTOR);
	ofs_z = (int8_t) round(mg_z / RSCS_ADXL345_OFFSET_SCALE_FACTOR);

	rscs_adxl345_setRegisterValue(device, RSCS_ADXL345_OFSX, ofs_x);
	rscs_adxl345_setRegisterValue(device, RSCS_ADXL345_OFSX, ofs_y);
	rscs_adxl345_setRegisterValue(device, RSCS_ADXL345_OFSX, ofs_z);

}


/* ЧТЕНИЕ ДАННЫХ ADXL345 В БИНАРНОМ ВИДЕ*/
void rscs_adxl345_read(rscs_adxl345_t * device, int16_t * x, int16_t * y, int16_t * z)
{
	uint8_t readBuffer[6]   = {0};

	rscs_adxl345_getRegisterValue(device,RSCS_ADXL345_DATAX0,readBuffer,6);

	*x = (readBuffer[1] << 8) | readBuffer[0];
	*y = (readBuffer[3] << 8) | readBuffer[2];
	*z = (readBuffer[5] << 8) | readBuffer[4];
}


void rscs_adxl345_cast_to_G(rscs_adxl345_t * device, int16_t x, int16_t y, int16_t z,
		float * x_g, float * y_g, float * z_g) {
	*x_g = x * 0.004f;
	*y_g = y * 0.004f;
	*z_g = z * 0.004f;
}


void rscs_adxl345_GetGXYZ(rscs_adxl345_t * device, int16_t* x, int16_t* y, int16_t* z, float* x_g, float* y_g, float* z_g)
{
	*x = 0;
	*y = 0;
	*z = 0;

	rscs_adxl345_read(device, x, y, z);
	rscs_adxl345_cast_to_G(device, *x, *y, *z, x_g, y_g, z_g);
}
