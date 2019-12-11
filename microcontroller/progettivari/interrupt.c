#include "stm32_unict_lib.h"


int value;

void setupLED(GPIO_TypeDef* gpio, int pin){
	value = 0;
	// LED at PC3
	GPIO_init(gpio);
	GPIO_config_output(gpio, pin);
}

void setupINTERRUPT(){
	// pushbutton Y (PB4)
		GPIO_init(GPIOB);
		GPIO_config_input(GPIOB, 4);
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
		GPIO_toggle(GPIOC, 3);
		EXTI_clear(EXTI4);
	}
}
