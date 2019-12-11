/*
 * uartgen.c
 *
 *  Created on: 11 nov 2019
 *      Author: giorgio
 */

#include "mialibstm32.h"
#include <stdio.h>

void aspetta_tick(unsigned tick){
	int i =0;
	while(i<tick){i++;}
}

int main(void)
{
	CONSOLE_init();
	DISPLAY_init();
//HAL_Delay(100);
	printf("H\n");
//	printf("e\n");
//	printf("l\n");
//	printf("l\n");
//	printf("o\n");
	while (1) {
		HAL_Delay(1000);
		if (kbhit()) {
			char s[5];
			char c = readchar();
			sprintf(s, "%4x", c);
			DISPLAY_puts(0, s);
		}

	}
}
