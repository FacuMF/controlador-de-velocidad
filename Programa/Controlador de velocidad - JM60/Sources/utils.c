/*
 * utils.c
 *
 *  Created on: Sep 17, 2020
 *      Author: Pablo
 */

#include "utils.h"

char cicloCompleto(char pulsos) {
	char res = (pulsos == 2) ? 1 : 0;
	if (pulsos < 2)
		pulsos++;
	else
		pulsos = 0;
	return res;
}

unsigned char* my_itoa(int number) {
	unsigned char str[4];
	sprintf(str, "%d", number);
	return str;
}


void delay(char segundos){
	TPM2SC = 0b00001111;
	switch(segundos){
	case 5: 
		TPM2MOD = 31249; //5s
		break;
	case 10:
		TPM2MOD = 62499; //1s
		break;		
		
	}
	while(!TPM2SC_TOF);
	TPM2SC_TOF = 0;
	TPM2SC = 0;
	
}

char determinarUso(int horas) {
	if (horas >= 0 && horas <= HORAS_BAJO)
		return BAJO;
	if (horas > HORAS_BAJO && horas <= HORAS_MEDIO)
		return MEDIO;
	if (horas > HORAS_MEDIO)
		return ALTO;
}

