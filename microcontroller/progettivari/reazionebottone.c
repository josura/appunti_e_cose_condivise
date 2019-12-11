/*
 * reazionebottone.c
 *
 *  Created on: 28 ott 2019
 *      Author: giorgio
 */


#include "stm32_unict_lib.h"
#include <stdio.h>
#include <stdlib.h>

int measure_done, timeout, started;

int main()
{
	DISPLAY_init();
	GPIO_init(GPIOB);
	GPIO_init(GPIOC);
	GPIO_config_output(GPIOB, 0); // 'red' LED
	GPIO_config_output(GPIOC, 2); // 'yellow' LED
	GPIO_config_input(GPIOB, 4); // tasto 'Y'
	GPIO_config_EXTI(GPIOB, EXTI4);
	EXTI_enable(EXTI4, FALLING_EDGE);

	GPIO_config_alternate(GPIOB, 4, 2); // button "Y" as AF2 --> TIM3,CH1
	TIM_init(TIM3);
	TIM_config_timebase(TIM3, 8400, 9999); // prescaler at 100us timeout of max secs
	TIM_config_IC(TIM3, 1, IC_RISING_EDGE); // capture on rising-edge (pushbutton)
	TIM_set(TIM3, 0);
	TIM_on(TIM3);
	TIM_enable_irq(TIM3, IRQ_UPDATE | IRQ_CC_1); // IRQ su UPDATE e su Capture canale 1

	for (;;) {
		started = 0;
		while (!started) { } // wait 'Y'
		GPIO_write(GPIOB, 0, 1); // turn on LED
		TIM_set(TIM3, 0); // clear timer and flags, start the counter
		measure_done = 0;
		timeout = 0;
		GPIO_write(GPIOC, 2, 1); // turn on LED
		DISPLAY_puts(0, "    ");

		while (!measure_done && !timeout) { } // wait for capture or timeout
		if (measure_done) {
			// capture OK!
			int ms = TIM3->CCR1 / 10;
			char s[5];
			sprintf(s,"%4d", ms);
			DISPLAY_puts(0, s);
		}
		else {
			// timeout
			DISPLAY_puts(0, "tout");
		}
	}
}

void TIM3_IRQHandler(void)
{
	if (TIM_update_check(TIM3)) {
		// period match, timeout
		GPIO_write(GPIOB, 0, 0);
		GPIO_write(GPIOC, 2, 0);
		timeout = 1;
		TIM_update_clear(TIM3);
	}
	if (TIM_cc_check(TIM3,1)) {
		// capture
		GPIO_write(GPIOB, 0, 0);
		GPIO_write(GPIOC, 2, 0);
		measure_done = 1;
		TIM_cc_clear(TIM3,1);
	}
}

void EXTI4_IRQHandler(void)    //tasto Y
{
	if (EXTI_isset(EXTI4)) {
		started = 1;
		EXTI_clear(EXTI4);
	}
}
