#include "stm32_unict_lib.h"



int flash_on = 0;

int light_index=0;
int lightvalues[] = { 50,100,150,200,250};

int main()
{
	// LED at PC3
	GPIO_init(GPIOC);
	GPIO_config_output(GPIOC, 3);
	// key 'X' at PB10
	GPIO_init(GPIOB);
	GPIO_config_input(GPIOB, 10);
	GPIO_config_EXTI(GPIOB, EXTI10);
	EXTI_enable(EXTI10, FALLING_EDGE);

	GPIO_config_input(GPIOB, 5);
	GPIO_config_EXTI(GPIOB, EXTI5);
	EXTI_enable(EXTI5, FALLING_EDGE);

	GPIO_config_input(GPIOB, 6);
	GPIO_config_EXTI(GPIOB, EXTI6);
	EXTI_enable(EXTI6, FALLING_EDGE);

	// init the timer
	TIM_init(TIM2);

	// Configure the timebase
	// Counter clock set to 0.1 ms
	TIM_config_timebase(TIM2, 8400, 5000);

	TIM_set(TIM2, 0); // resets the counter
	TIM_on(TIM2); // starts the timer

	TIM_enable_irq(TIM2, IRQ_UPDATE);

	// infinite loop
	for (;;) {
	}
}

void TIM2_IRQHandler(void)
{
	// check the update event
	if (TIM_update_check(TIM2)) {
		if (flash_on){
			if(GPIO_read(GPIOB,0)){
				GPIO_write(GPIOC, 0,0);
				TIM2->ARR=800;
			}
			else{
				GPIO_write(GPIOB,0,1);
				TIM2->ARR=lightvalues[light_index];
			}
		}
		else GPIO_write(GPIOC, 0, 1);
		// clear the update event
		TIM_update_clear(TIM2);

	}
}

void EXTI9_5_IRQHandler(void)
{
	if (EXTI_isset(EXTI5)) {
		light_index= (light_index+1)%5;
		EXTI_clear(EXTI5);
	} else if(EXTI_isset(EXTI6)){
		if(light_index==0)light_index=4;
		else light_index= light_index-1;
		EXTI_clear(EXTI6);
	}
}

void EXTI15_10_IRQHandler(void)
{
	if (EXTI_isset(EXTI10)) {
		flash_on =  !flash_on;
		EXTI_clear(EXTI10);
	}
}
