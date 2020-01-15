/*
 * implementazione_UART.c
 *
 *  Created on: 15 gen 2020
 *      Author: giorgio
 */

#include "stm32_unict_lib.h"
#include <stdlib.h>

#define GPIO_set_bit(gpio,pin,bit) gpio->ODR &=((~0)&(val<<pin))

void My_console_init(){
	GPIO_init(GPIOA);
	GPIO_config_output(GPIOA,2);
	TIM_init(TIM2);
	TIM_config_timebase(TIM2,42000,115);
	TIM_enable_irq(TIM2,IRQ_UPDATE);
	TIM_set(TIM2,0);
}

int transmitting;
int finished=1,currentbit;
int start;

void My_transmit_char(const char value){
	while(finished){
		finished=0;
		currentbit=0;
		transmitting=value;
		start=1;
		TIM_on(TIM2);
		while(!finished);
		TIM_off(TIM2);

	}
}

void My_transmit_string(const char * string){
	int current_char=0;

	while(string[current_char++]!='\0'){
		My_transmit_char(string[current_char]);
	}
}

int main(){
	GPIO_init(GPIOB);
	GPIO_config_output(GPIOB,0);

	DISPLAY_init();
	My_console_init();

	while(1){
		My_transmit_string("ciao\n");
	}

}


void TIM2_IRQHandler(){
	if(TIM_update_check(TIM2)){
		if(start){GPIO_write(GPIOA,2,0);start=0;}
		else if(currentbit<=7){
			currentbit++;
			int bit=(transmitting >> currentbit) & ((int32_t)1);
			char stringa[5];
			sprintf(stringa,"%i%i%i",currentbit,finished,bit);
			DISPLAY_puts(0,stringa);
			GPIO_write(GPIOA,2,bit);
		}
		if(currentbit > 7){
			GPIO_toggle(GPIOB,0);
			GPIO_write(GPIOA,2,1);
			finished=1;
		}
		TIM_update_clear(TIM2);
	}
}
