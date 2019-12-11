/*
 * compito18022019_using_registers.c
 *
 *  Created on: 18 nov 2019
 *      Author: giorgio
 */


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
	//GPIO_write(gpio,pin,1);
	gpio->ODR|=1<<pin;
	//GPIO_write(gpiono1,pinno1,0);
	gpiono1->ODR&=1<<pinno1;
	//GPIO_write(gpiono2,pinno2,0);
	gpiono2->ODR&=1<<pinno2;
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

	//GPIO_config_input(GPIOB, 10);
	GPIOB->MODER&=
	GPIO_config_input(GPIOB, 4);

	GPIO_config_EXTI(GPIOB, EXTI10);
	EXTI_enable(EXTI10, FALLING_EDGE); // X

	GPIO_config_EXTI(GPIOB, EXTI4);
	EXTI_enable(EXTI4, FALLING_EDGE); // Y


	GPIO_config_output(GPIOB, 0);
	GPIO_config_output(GPIOC, 2);
	GPIO_config_output(GPIOC, 3);

	configureADC();
