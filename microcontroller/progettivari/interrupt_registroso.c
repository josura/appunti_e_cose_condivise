/*
 * interrupt_registroso.c
 *
 *  Created on: 18 nov 2019
 *      Author: giorgio
 */


#include "stm32_unict_lib.h"

enum{INPUT,OUTPUT,ALTERNATE,ANALOG};



GPIO_config_mio(int tipo, GPIO_TypeDef* gpio,int pin){
	int val=0;
	switch (tipo){
	case INPUT:
		val=0;
		break;
	case OUTPUT:
		val=1;
		break;
	case ALTERNATE:
		val=2;
		break;
	case ANALOG:
		val=3;
		break;
	}
	gpio->MODER&=~(3<<pin*2);
	//azzeramento
	gpio->MODER|=val<<pin*2;
	//configurazione di tipo
}

void setupLED(GPIO_TypeDef* gpio, int pin){
	value = 0;
	// LED at PC3
	GPIO_init(gpio);
	//GPIO_config_output(gpio, pin);
	gpio->MODER&=~(3<<pin*2);
	//azzeramento
	gpio->MODER|=1<<pin*2;
	//configurazione di output  01
}

void setupINTERRUPT(){
	// pushbutton Y (PB4)
		GPIO_init(GPIOB);
		//GPIO_config_input(GPIOB,4);
		GPIO_config_mio(INPUT,GPIOB,4);
		GPIO_config_EXTI(GPIOB, EXTI4);
		EXTI_enable(EXTI4, FALLING_EDGE);

}

int main(){
	setupLED(GPIOC, 3);
	setupINTERRUPT();
	for (;;) { } // do nothing
}

void EXTI4_IRQHandler(void){
	if (EXTI_isset(EXTI4)) {
		//GPIO_toggle(GPIOC, 3);
		int maschera_fin_toogle=((~(GPIOC->ODR&(3<<3*2)))&(3<<3*2));
		GPIOC&=~(3<<3*2);
		//azzeramento
		GPIOC|=maschera_fin_toogle;
		//assegnazione del valore togglato
		EXTI_clear(EXTI4);
	}
}
