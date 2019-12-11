#include "stm32_unict_lib.h"
#include <stdio.h>
#include <string.h>

int freqlamp=0;
int main()
{
	DISPLAY_init();
	GPIO_init(GPIOB);
	GPIO_config_alternate(GPIOB, 0, 2); // AF2 --> TIMER3, CH3

	TIM_init(TIM3);
	// f = 84MHz
	// PSC = 60000
	// frequenza di incremento = 84Mhz/420 = 200000Hz forse
	// durata di un tick di incremento ~= 0.7ms
	// Periodo = 600ms --> ~ 858 count
	ADC_init(ADC1, ADC_RES_10, ADC_ALIGN_RIGHT);
	ADC_channel_config(ADC1, GPIOC, 0, 10);
	ADC_on(ADC1);
	TIM_config_timebase(TIM3, 420, 1000);
	TIM_config_PWM(TIM3, 3); // TIMER3, CH3
	TIM_set(TIM3, 0);
	TIM_on(TIM3);

	for (;;) {
		ADC_sample_channel(ADC1, 10);
		ADC_start(ADC1);
		while (!ADC_completed(ADC1)) {}
		freqlamp = (ADC_read(ADC1)*1000)/1024;
		// Ton = 50ms -> ~ 72 count
		TIM3->CCR3 = freqlamp;
		char s[5];
		sprintf(s, "%4d", freqlamp); // we will display the milliseconds
		DISPLAY_puts(0,s);
	}
}
