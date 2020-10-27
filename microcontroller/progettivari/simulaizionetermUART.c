/*
 * simulaizionetermUART.c
 *
 *  Created on: 13 nov 2019
 *      Author: giorgio
 */


#include "stm32_unict_lib.h"
#include <stdio.h>
#include <string.h>


void getstring(char * s, int maxlen)
{
	int i = 0;
	for (;;) {
		char c = readchar();
		switch (c) {
		case 13:
			printf("\n");
			s[i] = 0;
			return;
		case 8:
			if (i > 0) {
				--i;
				__io_putchar(8); // BS
				__io_putchar(' '); // SPAZIO
				__io_putchar(8); // BS
			}
			break;
		default: // il carattere appartiene al set stampabile
			if (c>=32 && i < maxlen) {
				__io_putchar(c); // echo del carattere appena inserito
				// inserisci il carattere nella stringa
				s[i] = c;
				i++;
			}
		}
	}
}

int main(void)
{
	CONSOLE_init();

	for (;;) {
		char s[20];
		int i;
		printf("Inserisci una stringa:");
		fflush(stdout);
		getstring(s, 19);
		printf("Stringa inserita: %s\n", s);
		for (i = 0; i < strlen(s);i++) {
			printf("%d ", s[i]);
		}
		printf("\n");
	}
}
