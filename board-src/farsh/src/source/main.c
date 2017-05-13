#include "all_includes.h"
#include "sensors.h"
#include "telemetry.h"

rscs_uart_bus_t *uart1;

int init_uart_stdout()
{
	uart1 = rscs_uart_init(RSCS_UART_ID_UART0, 	RSCS_UART_FLAG_ENABLE_TX |
												RSCS_UART_FLAG_BUFFER_TX |
												RSCS_UART_FLAG_ENABLE_RX |
												RSCS_UART_FLAG_BUFFER_RX);
	if (!uart1)
		return 1;

	rscs_uart_set_baudrate(uart1, 9600);
	rscs_uart_set_character_size(uart1, 8);
	rscs_uart_set_parity(uart1, RSCS_UART_PARITY_NONE);
	rscs_uart_set_stop_bits(uart1, RSCS_UART_STOP_BITS_ONE);

	stdout = rscs_make_uart_stream(uart1);

	return 0;
}

void servo_oriantate()
{
	uint8_t counter[4] = {0};
	uint8_t maxn = 0;
	int16_t max = 0;

	for(int i = 0; i < 5; i++)
	{
		update_status();
		for(int j = 4; j < 8; j++)
		{
			if(max < status.lights[j])
			{
				max = status.lights[j];
				maxn = j - 4;
			}
		}
		counter[maxn]++;
		}
		maxn = 0;
		max = 0;
		for(int i = 0; i < 4; i++)
		{
			if(max < counter[i])
			{
				max = counter[i];
				maxn = i;
			}
	}
	rscs_servo_set_angle(0,maxn * 60);
}

void config_radio_check()
{
	//DDRA |= (1<<1);
	//PORTA |= (1<<1);

	/*DDRA &=~ (1<<0);
	PORTA |= (1<<0);

	if (0 == (PINA & (1<<0))){
		DDRA |= (1<<1);
		PORTA &=~ (1<<1);
		while(1){};
	}

	DDRA |= (1<<1);
	PORTA |= (1<<1);*/
}

void hardware_init()
{
	//config_radio_check();

	sei();

	//init_uart_radio();
	init_uart_stdout();

	rscs_i2c_init();
	rscs_i2c_set_scl_rate(17);
	//rscs_ow_init_bus();

	rscs_servo_init(1);

	ads_init();
	//bmp_init();
	//adxl_init();
	//ds_init();
}

int main()
{
	hardware_init();
	rscs_servo_timer_init();

	while(1){
		servo_oriantate();
	}
	return 0;
}



/*void acs712_test(){
	int error = 0;
	int32_t raw_data;

	rscs_adc_init();
	rscs_adc_set_refrence(RSCS_ADC_REF_EXTERNAL_VCC);
	rscs_adc_set_prescaler(RSCS_ADC_PRESCALER_128);
	error = rscs_adc_start_continuous_conversion(RSCS_ADC_SINGLE_0);
	printf("%d\n", error);
	printf("Hi\n");

	while (true){
		error = rscs_adc_get_result(&raw_data);
		if (error == RSCS_E_BUSY) {printf("%d;  ", error);}
			else{
				printf("%ld\n", raw_data);
			}
		_delay_ms(100);
	}
}*/
