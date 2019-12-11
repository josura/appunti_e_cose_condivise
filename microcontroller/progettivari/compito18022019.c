#include "stm32_unict_lib.h"
#include <stdio.h>

enum {SETUP=1,RUN};


int state=RUN;
int turbolenza=0;
int rottadesiderata=0;
float rottaeff=0;

int select_rotta(float val_ADC){
	int nuovotipo=(int)((val_ADC/4095.0)*360)-180;
	return nuovotipo;
}

int select_turbolenza(float val_ADC){
	int TMAX=rand()%3;
	int nuovotipo=(int)((val_ADC/4095.0)*(TMAX*2))-TMAX;
	return nuovotipo;
}

void display_rotta(int tipo){
	char str[5];
	switch(state){
	case SETUP:
		sprintf(str,"%4i",tipo);
		DISPLAY_puts(0,str);
		break;
	case RUN:
		sprintf(str,"%4i",tipo);
		DISPLAY_puts(0,str);
		break;
	default:
		DISPLAY_puts(0,"-666");
		break;
	}
}

int alettoni(){
	int alettonimax= (int)((rottadesiderata- rottaeff)*0.08);
	if(alettonimax>10){
		return 10;
	}else if(alettonimax<-10){
		return -10;
	}else return alettonimax;
}

void solounaluce(GPIO_TypeDef* gpio,int pin,GPIO_TypeDef* gpiono1,int pinno1,GPIO_TypeDef* gpiono2,int pinno2){
	GPIO_write(gpio,pin,1);
	GPIO_write(gpiono1,pinno1,0);
	GPIO_write(gpiono2,pinno2,0);
}


void configureADC(){
	//inizializzamo il convertitore
	ADC_init(ADC1, ADC_RES_12, ADC_ALIGN_RIGHT);
	//PC0 primo potenziometro
	ADC_channel_config(ADC1, GPIOC, 0, 10);
	//PC1 secondo potenziometro
	ADC_channel_config(ADC1, GPIOC, 1, 11);
	//attiviamo il convertitore
	ADC_on(ADC1);

}

int main(void)
{
	GPIO_init(GPIOB);
	GPIO_init(GPIOC);
	DISPLAY_init();

	GPIO_config_input(GPIOB, 10);
	GPIO_config_input(GPIOB, 4);

	GPIO_config_EXTI(GPIOB, EXTI10);
	EXTI_enable(EXTI10, FALLING_EDGE); // X

	GPIO_config_EXTI(GPIOB, EXTI4);
	EXTI_enable(EXTI4, FALLING_EDGE); // Y


	GPIO_config_output(GPIOB, 0);
	GPIO_config_output(GPIOC, 2);
	GPIO_config_output(GPIOC, 3);

	configureADC();

	TIM_init(TIM1);
	TIM_config_timebase(TIM2, 8400, 100);
	TIM_set(TIM1, 0);
	TIM_enable_irq(TIM1, IRQ_UPDATE);
	TIM_on(TIM1);

	TIM_init(TIM2);
	TIM_config_timebase(TIM2, 8400, 1000);
	TIM_set(TIM2, 0);
	TIM_enable_irq(TIM2, IRQ_UPDATE);
	TIM_on(TIM2);
	GPIO_write(GPIOC,3,0);
	ADC_sample_channel(ADC1, 11);
	ADC_start(ADC1);
	while (!ADC_completed(ADC1)) {}
	rottadesiderata = ADC_read(ADC1);
	while(1) {
		switch(state){
		case RUN:
			display_rotta(rottaeff);
			break;
		case SETUP:
			GPIO_write(GPIOC,3,0);
			ADC_sample_channel(ADC1, 11);
			ADC_start(ADC1);
			while (!ADC_completed(ADC1)) {}
			display_rotta(select_rotta(ADC_read(ADC1)));
			break;
		}
	}

}

void TIM1_IRQHandler(){
	if (TIM_update_check(TIM1)) {		//timer conteggio rotta
		switch(state){
		case RUN:
			ADC_sample_channel(ADC1, 10);
			ADC_start(ADC1);
			while (!ADC_completed(ADC1)) {}
			turbolenza =select_turbolenza(ADC_read(ADC1));
			rottaeff+=alettoni()*0.04+turbolenza;
			if(rottaeff>180)rottaeff=180;
			else if(rottaeff<-180)rottaeff=-180;
			if((rottadesiderata-rottaeff)<2){
				GPIO_write(GPIOC,3,1);
			}
			else {
				GPIO_write(GPIOC,3,0);
			}
			break;
		case SETUP:
			break;
		default:
			break;
		}
			TIM_update_clear(TIM2);
	}
}

void TIM2_IRQHandler(void)
{
	if (TIM_update_check(TIM2)) {
		switch(state){
		case RUN:
			break;
		case SETUP:
			break;
		default:
			break;
		}
		TIM_update_clear(TIM2);
	}

}

void EXTI4_IRQHandler(void){   //tasto Y
	if (EXTI_isset(EXTI4)) {
		switch(state){
		case RUN:
			break;
		case SETUP:
			GPIO_write(GPIOC,3,0);
			ADC_sample_channel(ADC1, 11);
			ADC_start(ADC1);
			while (!ADC_completed(ADC1)) {}
			rottadesiderata = ADC_read(ADC1);
			state=RUN;
			break;
		default:
			break;
		}
		EXTI_clear(EXTI4);
	}
}

void EXTI15_10_IRQHandler(void)
{
	if (EXTI_isset(EXTI10)) {    //tasto X
		switch(state){
		case RUN:
			state = SETUP;
			break;
		case SETUP:
			state=RUN;
			break;
		default:
			break;
		}
		EXTI_clear(EXTI10);
	}
}
