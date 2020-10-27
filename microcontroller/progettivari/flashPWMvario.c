/*
 * flashPWMvario.c
 *
 *  Created on: 27 nov 2019
 *      Author: giorgio
 */




#include <stdio.h>
#include "stm32_unict_lib.h"
int new_arr_value = 25;

void configureADC(){
	ADC_init(ADC1, ADC_RES_10, ADC_ALIGN_RIGHT);
	//PC0 primo potenziometro
	ADC_channel_config(ADC1, GPIOC, 0, 10);
	//PC1 secondo potenziometro
	ADC_channel_config(ADC1, GPIOC, 1, 11);
	ADC_on(ADC1);

}

int timeron=1;

int main(void)
{
	DISPLAY_init();
	GPIO_init(GPIOB);
	GPIO_config_alternate(GPIOB, 8,2 ); // AF2 --> TIMER4, CH3
	CONSOLE_init();
	printf("configurazione iniziale finita\n");
	TIM_init(TIM4);
	TIM_config_timebase(TIM4, 420, 1000);
	TIM_config_PWM(TIM4, 3); // TIMER3, CH3
	TIM_set(TIM4, 0);
	TIM_on(TIM4);
	printf("configurazione PWM finita\n");
	//timer per la luce lampeggiante
	TIM_init(TIM2);
	printf("ci siamo ?");
	TIM_config_timebase(TIM2, 8400, 100); //ogni secondo all'inizio
	TIM_set(TIM2, 0);
	printf("configurazione timer 2");
	TIM_enable_irq(TIM2, IRQ_UPDATE);
	TIM_on(TIM2);
	printf("configurazione timer 2 irq");
	/*for (;;) {
		//duty cycle
		ADC_sample_channel(ADC1, 11);
		ADC_start(ADC1);
		while (!ADC_completed(ADC1)) { }
		int freqpwm = (ADC_read(ADC1)*1000)/1023;       //
		TIM4->CCR3=freqpwm;
		//nuovo tempo periodo
		ADC_sample_channel(ADC1, 10);
		ADC_start(ADC1);
		while (!ADC_completed(ADC1)) { }
		int periodo= (ADC_read(ADC1)*(100-10)/1023+10);   //periodo nell'intervallo tra 100 e 1000 ms
		TIM2->ARR=periodo;
		char s[5];
		sprintf(s, "%4d", new_arr_value); // we will display the milliseconds
		DISPLAY_puts(0,s);
	}*/
}


void TIM2_IRQHandler(void)
{
	if (TIM_update_check(TIM2)) {
		if(timeron){
			TIM_off(TIM4);
			timeron=0;
		}else {
			TIM_on(TIM4);
			timeron=1;
		}
		TIM_update_clear(TIM2)
	}
}

