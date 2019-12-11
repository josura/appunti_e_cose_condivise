/*
 * proviamo.c
 *
 *  Created on: 28 ott 2019
 *      Author: giorgio
 */


#include "mialibstm32.h"
#include <stdio.h>

enum {SELECT=1,EROGATION};
enum {CAFFE,TE,CIOC};


int new_arr_value = 25;
int state=SELECT;
int prezzo=0;
int pagamento=0,resto=0;
int tempomoneta=0;
int ledon=0;
int mon5=10,mon10=10,mon20=10;
int erogon=0;

int select_tipo(float val_ADC){
	int nuovotipo=(int)((val_ADC/255.0)*2.01);
	return nuovotipo;
}

void solounaluce(GPIO_TypeDef* gpio,int pin,GPIO_TypeDef* gpiono1,int pinno1,GPIO_TypeDef* gpiono2,int pinno2){
	GPIO_write(gpio,pin,1);
	GPIO_write(gpiono1,pinno1,0);
	GPIO_write(gpiono2,pinno2,0);
}

void display_bevanda(int tipo){
	char str[5];
	switch(tipo){
	case CAFFE:
		sprintf(str,"%2iCa",prezzo-pagamento);
		DISPLAY_puts(0,str);
		//solounaluce(GPIOB,0,GPIOC,2,GPIOC,3);
		prezzo=35;
		break;
	case TE:
		prezzo=40;
		//solounaluce(GPIOC,2,GPIOB,0,GPIOC,3);
		sprintf(str,"%2ite",prezzo-pagamento);
		DISPLAY_puts(0,str);
		break;
	case CIOC:
		prezzo=45;
		//solounaluce(GPIOC,3,GPIOC,2,GPIOB,0);
		sprintf(str,"%2iCi",prezzo-pagamento);
		DISPLAY_puts(0,str);
		break;
	}

}

int main(void)
{
	GPIO_init(GPIOB);
	GPIO_init(GPIOC);
	DISPLAY_init();

	GPIO_config_input(GPIOB, 10);
	GPIO_config_input(GPIOB, 4);
	GPIO_config_input(GPIOB, 5);
	GPIO_config_input(GPIOB, 6);

	GPIO_config_EXTI(GPIOB, EXTI10);
	EXTI_enable(EXTI10, FALLING_EDGE); // X

	GPIO_config_EXTI(GPIOB, EXTI4);
	EXTI_enable(EXTI4, FALLING_EDGE); // Y

	GPIO_config_EXTI(GPIOB, EXTI5);
	EXTI_enable(EXTI5, FALLING_EDGE); // Z

	GPIO_config_EXTI(GPIOB, EXTI6);
	EXTI_enable(EXTI6, FALLING_EDGE); // T

	GPIO_config_output(GPIOB, 0);
	GPIO_config_output(GPIOC, 2);
	GPIO_config_output(GPIOC, 3);


	//configurazione ADC potenziometro su GPIOC0
	ADC_init(ADC1, ADC_RES_8, ADC_ALIGN_RIGHT);
	ADC_channel_config(ADC1, GPIOC, 0, 10);
	ADC_on(ADC1); ADC_sample_channel(ADC1, 10);

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
	for (;;) {
		ADC_start(ADC1);
		while (!ADC_completed(ADC1)) {}
		int value = ADC_read(ADC1);
		display_bevanda(select_tipo(value));
		if(state==SELECT){
			if(tempomoneta>0){
				tempomoneta--;
				//HAL_Delay(1);
				if(!ledon){
					GPIO_write(GPIOB, 0,1);
					ledon=1;
				}
			}
			else{
				GPIO_write(GPIOB,0,0);
				ledon=0;
			}
		}
		else{
			while(resto>0){
				if(resto>=20 && mon20>0){
					resto-=20;
					mon20--;
				}
				else if(resto >= 10 && mon10>0){
					resto-=10;
					mon10--;
				}
				else if(resto >= 5 && mon5>0){
					resto-=5;
					mon5--;
				}
			}
			//HAL_Delay(100);
			state=SELECT;
		}
	}
}

void TIM1_IRQHandler(){
	if (TIM_update_check(TIM1)) {		//timer monete
		if(tempomoneta>0){
				tempomoneta--;
				if(!ledon){
					GPIO_toggle(GPIOB, 0);
					ledon=1;
				}
			}
			else{
				ledon=0;
			}
		switch(state){
				case SELECT:

					break;

				case EROGATION:


					break;
				default:
					break;
					}
			TIM_update_clear(TIM1);
	}
}

void TIM2_IRQHandler(void)
{
	if (TIM_update_check(TIM2)) {            //timer erogazione
		switch(state){
			case SELECT:
				GPIO_write(GPIOC,2,0);

				break;
			case EROGATION:
					GPIO_write(GPIOC,2,1);
					state  = SELECT;


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
				case SELECT:
					pagamento+=20;
					tempomoneta=500;
					mon20++;
					if(pagamento>=prezzo){
						resto=pagamento-prezzo;
					}
					break;
				case EROGATION:
					break;
				default:
					break;
				}
		EXTI_clear(EXTI4);
	}
}

void EXTI9_5_IRQHandler(void)
{
	if (EXTI_isset(EXTI5)) {   //tasto Z
		switch(state){
				case SELECT:
					tempomoneta=500;
					pagamento+=10;
					mon10++;
					if(pagamento>=prezzo){
						resto = pagamento - prezzo;
					}
					break;
				case EROGATION:
					break;
				default:
					break;
				}
		EXTI_clear(EXTI5);
	}
	if (EXTI_isset(EXTI6)) { //'T'		erogazioneattivata
		switch(state){
			case SELECT:
				if(pagamento - prezzo <0){
					GPIO_write(GPIOC,3,1);
					break;
				}
				else {
					GPIO_write(GPIOC,3,0);
				}
				state=EROGATION;
				erogon=1;
				GPIO_write(GPIOC,2,1);
				pagamento=0;
				TIM2->CNT=0;
				break;
			case EROGATION:
				break;
			default:
				break;
			}
		EXTI_clear(EXTI6);
	}
}

void EXTI15_10_IRQHandler(void)
{
	if (EXTI_isset(EXTI10)) {    //tasto X
		switch(state){
		case SELECT:
			pagamento+=50;
			tempomoneta=500;
			if(pagamento>=prezzo){
				resto=pagamento - prezzo;
			}
			break;
		case EROGATION:
			break;
		default:
			break;
		}
		EXTI_clear(EXTI10);
	}
}

