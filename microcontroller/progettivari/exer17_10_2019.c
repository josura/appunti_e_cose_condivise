/*
 * exer17_10_2019.c
 *
 *  Created on: 16 ott 2019
 *      Author: giorgio
 */

#include "stm32_unict_lib.h"

int percap=0;
int timer_att=0;
int timer_attesa=0,timer_opencl=0;

enum {CLOSED,OPENING,OPEN,CLOSING};

int state=CLOSED;

void showgate(){
	if(percap==timer_opencl ){
		DISPLAY_puts(0,"    ");
	}else if(percap>0.75*timer_opencl){
		DISPLAY_puts(0,"-   ");
	} else if(percap>0.5*timer_opencl){
		DISPLAY_puts(0,"--  ");
	} else if(percap>0.25*timer_opencl){
		DISPLAY_puts(0,"--- ");
	}else if(percap>=0){
		DISPLAY_puts(0,"----");
	}
}

void configureADC(){
	ADC_init(ADC1, ADC_RES_8, ADC_ALIGN_RIGHT);
	//PC0 primo potenziometro
	ADC_channel_config(ADC1, GPIOC, 0, 10);
	//PC1 secondo potenziometro
	ADC_channel_config(ADC1, GPIOC, 1, 11);
	ADC_on(ADC1);

}

int main(void)
{
    GPIO_init(GPIOB);
    DISPLAY_init();

    GPIO_config_output(GPIOB, 0);

    GPIO_config_input(GPIOB, 10);
    GPIO_config_input(GPIOB, 4);
    GPIO_config_input(GPIOB, 5);

    GPIO_config_EXTI(GPIOB, EXTI10);
    EXTI_enable(EXTI10, FALLING_EDGE); // X

    GPIO_config_EXTI(GPIOB, EXTI4);
    EXTI_enable(EXTI4, FALLING_EDGE); // Y

    GPIO_config_EXTI(GPIOB, EXTI5);
    EXTI_enable(EXTI5, FALLING_EDGE); // Z

    TIM_init(TIM2);

    // Configure the timebase
    // Counter clock set to 0.1 ms
    TIM_config_timebase(TIM2, 8400, 1000);

    TIM_set(TIM2, 0); // resets the counter
    TIM_on(TIM2); // starts the timer

    TIM_enable_irq(TIM2, IRQ_UPDATE);
    configureADC();

    while (1) {
    	//tempo apertura chiusura
    	ADC_sample_channel(ADC1, 11);
		ADC_start(ADC1);
		while (!ADC_completed(ADC1)) { }
		int value = ADC_read(ADC1);
		timer_opencl = (value*(10-6)/255+6)*10;
		//tempo attesa
		ADC_sample_channel(ADC1, 10);
		ADC_start(ADC1);
		while (!ADC_completed(ADC1)) { }
		value = ADC_read(ADC1);
		timer_attesa = (value*(15-3)/255+3)*10;
		showgate();
    }

}

void TIM2_IRQHandler(void)
{
	// check the update event
	if (TIM_update_check(TIM2)) {
		switch(state){
		case CLOSED:
			GPIO_write(GPIOB, 0, 0);
			percap=0;
			break;
		case OPENING:
			GPIO_toggle(GPIOB, 0);
			percap++;
			if(percap>=timer_opencl) {
				state=OPEN;
				timer_att=0;
			}
			break;
		case OPEN:
			GPIO_write(GPIOB,0,0);
			timer_att++;
			if(timer_att>=timer_attesa){
				state= CLOSING;
				timer_att=0;
			}
			break;
		case CLOSING:
			GPIO_toggle(GPIOB, 0);
			percap--;
			if(percap<=0)state=CLOSED;
			break;
		}
		TIM_update_clear(TIM2);
	}
}

void EXTI4_IRQHandler(void){   //tasto Y
	if (EXTI_isset(EXTI4)) {
		switch(state){
		case CLOSED:
			//niente
			break;
		case OPENING:
			//niente
			break;
		case OPEN:
			state= CLOSING;
			break;
		case CLOSING:
			//niente
			break;
		}
		EXTI_clear(EXTI4);
	}
}

void EXTI9_5_IRQHandler(void)
{
	if (EXTI_isset(EXTI5)) {   //tasto Z
		switch(state){
		case CLOSED:
			//niente
			break;
		case OPENING:
			//niente
			break;
		case OPEN:
			timer_att=0;
			break;
		case CLOSING:
			state=OPENING;
			timer_att=0;
			break;
		}
		EXTI_clear(EXTI5);
	}
}

void EXTI15_10_IRQHandler(void)
{
	if (EXTI_isset(EXTI10)) {    //tasto X
		switch(state){
		case CLOSED:
			state=OPENING;
			break;
		case OPENING:
			//niente
			break;
		case OPEN:
			timer_att=0;
			break;
		case CLOSING:
			state=OPENING;
			break;
		}
		EXTI_clear(EXTI10);
	}
}
