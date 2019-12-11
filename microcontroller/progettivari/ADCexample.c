/*
 * ADCexample.c
 *
 *  Created on: 21 ott 2019
 *      Author: giorgio
 */


#include <stdio.h>
#include "stm32_unict_lib.h"
int new_arr_value = 100;
int main(void)
{
	DISPLAY_init();
	GPIO_init(GPIOB); GPIO_config_output(GPIOB, 0);
	ADC_init(ADC1, ADC_RES_8, ADC_ALIGN_RIGHT);
	ADC_channel_config(ADC1, GPIOC, 0, 10);
	ADC_on(ADC1); ADC_sample_channel(ADC1, 10);
	TIM_init(TIM2);
	TIM_config_timebase(TIM2, 42000, 100);
	TIM_set(TIM2, 0);
	TIM_enable_irq(TIM2, IRQ_UPDATE);
	TIM_on(TIM2);
	for (;;) {
		ADC_start(ADC1);
		while (!ADC_completed(ADC1)) {}
		int value = ADC_read(ADC1);
		new_arr_value = value * 900/255 + 100;
		char s[5];
		sprintf(s, "%4d", new_arr_value/2); // we will display the milliseconds
		DISPLAY_puts(0,s);
	}
}

void TIM2_IRQHandler(void)
{
	if (TIM_update_check(TIM2)) {
		GPIO_toggle(GPIOB, 0);
		TIM_update_clear(TIM2);
		TIM2->ARR = new_arr_value;
	// update the autoreload register with new value
	}
}
