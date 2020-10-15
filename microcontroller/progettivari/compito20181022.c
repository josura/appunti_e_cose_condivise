#include "stm32_unict_lib.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

enum {
	ST_OFF,
	ST_ON,
	SETUP
};

typedef struct {
	int sec, cent;
} t_time;

int state =ST_OFF;
t_time timeout={0,0};
t_time currenttime={0,0};

int decrementTime(t_time* t){
	t->cent--;
	if(t->cent==-1){
		t->sec--;
		t->cent=99;
	}
	if(t->cent==0 && t->sec==0) return -1;
	else return t->sec;
}

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


void showtime(t_time* t){
	char stringa[6];
	sprintf(stringa,"%2i%2i",t->sec,t->cent);
	DISPLAY_puts(0,stringa);
}

void showbars(){
	DISPLAY_puts(0,"----");
}

void flashingLed(){
	GPIO_write(GPIOC,3,1);
	delay_ms(250);
	GPIO_write(GPIOC,3,0);
	delay_ms(250);
	GPIO_write(GPIOC,3,1);
}

int main(){
	DISPLAY_init();
	GPIO_init(GPIOC);
	GPIO_config_output(GPIOC, 3);
	while(1){
		switch(state){
		case ST_ON:
			showtime(&currenttime);
			delay_ms(1000);
			if(decrementTime(&currenttime)<0){
				state=ST_OFF;
			}
			break;
		case ST_OFF:
			showbars();
			break;
		case SETUP:
			showtime(&timeout);
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
		case ST_OFF:       //da OFF a ON
			state = ST_ON;
			currenttime =timeout;
			break;
		case ST_ON:			//resetta il contatore
			currenttime = timeout;
			break;
		case SETUP:		//non fa niente
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
		if (state == ST_OFF) {    //da ST_OFF a SETUP
			state = SETUP;
		}
		else if (state == SETUP) {	//da SETUP a ST_OFF
			state = ST_OFF;
		}
		EXTI_clear(EXTI4);
	}
}


// tasto 'Z' e 'T'
void EXTI9_5_IRQHandler(void)
{
	if (EXTI_isset(EXTI5)) { //'Z'     diminuisce il timeout se è nello stato di SETUP
		if (state == SETUP) {
			if (timeout.sec > 5)
				timeout.sec--;
		}
		EXTI_clear(EXTI5);
	}
	if (EXTI_isset(EXTI6)) { //'T'		aumenta il timeout se è nello stato di SETUP
		if (state == SETUP) {
			if (timeout.sec < 20)
				timeout.sec++;
		}
		EXTI_clear(EXTI6);
	}
}

