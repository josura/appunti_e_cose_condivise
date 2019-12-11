#include "stm32_unict_lib.h"
#include "stdio.h"

enum {
	SELECT,
	EROGATION
};



int state =SELECT;
float import = 0;
float carburante=0;
int time = 0;

void initDuePunti(){
	GPIO_init(GPIOB);
	GPIO_config_output(GPIOB, 8);
}
void onDuePunti(){
	GPIO_write(GPIOB,8,1);
}

void offDuePunti(){
	GPIO_write(GPIOB,8,0);
}


void showimport(float* t){
	char stringa[6];
	int intera=(int)(*t *100);
	sprintf(stringa,"%4i",intera);
	DISPLAY_puts(0,stringa);
}

void showcarburante(int* t){
	char stringa[5];
	sprintf(stringa,"%i",*t);
	DISPLAY_puts(0,stringa);  //dove lo mettiamo il carburante
}

void showbars(){
	DISPLAY_puts(0,"----");
}

void flashingYellowLed(){
	if((time++)<500)
		GPIO_write(GPIOC,2,1);
	else GPIO_write(GPIOC,2,0);
	if(time>=1000) time=0;
}

void RedLed(){
	GPIO_write(GPIOB,0,1);
	GPIO_write(GPIOC, 2, 0);
	while((state == EROGATION) && import <= 0);
	GPIO_write(GPIOB,0,0);
}

int main(){
	DISPLAY_init();
	GPIO_init(GPIOC);
	GPIO_init(GPIOB);
	GPIO_config_output(GPIOC, 2);
	GPIO_config_output(GPIOB, 0);
	GPIO_config_input(GPIOB, 4);
	GPIO_config_EXTI(GPIOB, EXTI4);
	EXTI_enable(EXTI4, FALLING_EDGE);
	GPIO_config_EXTI(GPIOB, EXTI10);
	EXTI_enable(EXTI10, FALLING_EDGE);
	GPIO_config_EXTI(GPIOB, EXTI5);
	EXTI_enable(EXTI5, FALLING_EDGE);
	GPIO_config_EXTI(GPIOB, EXTI6);
	EXTI_enable(EXTI6, FALLING_EDGE);
	TIM_init(TIM2);

	// Configure the timebase
	// Counter clock set to 0.1 ms
	TIM_config_timebase(TIM2, 8400, 5000);

	TIM_set(TIM2, 0); // resets the counter
	TIM_on(TIM2); // starts the timer

	TIM_enable_irq(TIM2, IRQ_UPDATE);
	while(1){
		switch(state){
		case SELECT:
			showimport(&import);
			break;
		case EROGATION:
			showimport(&import);
			//showcarburante((int)&((int) carburante));
			if(state == EROGATION){
						if(import > 0){
							delay_ms(1);
							import -= 0.00075;
							carburante += 0.0005;
							flashingYellowLed();
						}
					}
			if(import <1.0) RedLed();
			break;
		default:
			break;
		}
	}
}

// tasto 'X'
void EXTI15_10_IRQHandler(void)
{
	if (EXTI_isset(EXTI10)) {
		switch (state) {
		case SELECT:
			import += 20.0;
			break;
		case EROGATION:			//resetta il contatore
			break;
		default:		//niente
			break;
		}
		EXTI_clear(EXTI10);
	}
}

// tasto 'Y'
void EXTI4_IRQHandler(void)
{
	if (EXTI_isset(EXTI4)) {
		switch(state){
			case SELECT:
				import += 5.0;
				break;
			case EROGATION:
				break;
			default:		//niente
				break;
		}
	}
	EXTI_clear(EXTI4);
}


// tasto 'Z' e 'T'
void EXTI9_5_IRQHandler(void)
{
	if (EXTI_isset(EXTI5)) { //'Z'     RESETTA IMPORT
		if (state == SELECT) {
			import = 0;
		}
		EXTI_clear(EXTI5);
	}
	if (EXTI_isset(EXTI6)) { //'T'		entra in EROGATION O ESCI DA EROGATION SE IMPORT <=0
		if (state == SELECT) {
			state = EROGATION;
			carburante = 0;
		}
		else if (state == EROGATION && import <= 0){
			state =SELECT;
			import =0;
		}
		EXTI_clear(EXTI6);
	}
}

void TIM2_IRQHandler(void)
{
	// check the update event
	if (TIM_update_check(TIM2)) {
		/*if(state == EROGATION){
			if(import > 0){
				import -= 0.000075;
				carburante += 0.00005;
				flashingYellowLed();
			}
		}*/
		TIM_update_clear(TIM2);
	}
}
