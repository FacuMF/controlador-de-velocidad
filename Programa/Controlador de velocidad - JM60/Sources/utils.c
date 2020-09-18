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

void mostrarNumero(unsigned char digitoMostrado){
	int i;
	_DATA = SALIDA;
	_CLK = SALIDA;
	DATA = APAGADO;
	CLK = APAGADO;
	
	for (i = 0; i <= 7; i++) {
		DATA = digitoMostrado & 1;
		CLK = 1;
		CLK = 0;
		digitoMostrado = digitoMostrado >> 1;
	}
}

void mostrarHorasEnDisplay(int horas){
	int horasAMostrar[4];
	char i;
	horasAMostrar[0] = horas/1000;
	horasAMostrar[1] = horas/100 - horasAMostrar[0];
	horasAMostrar[2] = horas/10 - horasAMostrar[0] - horasAMostrar[1];
	horasAMostrar[3] = horas - horasAMostrar[0] - horasAMostrar[1] - horasAMostrar[2];
	
	for(i = 0; i<4; i++){
		mostrarNumero(horasAMostrar[i]);
		delay(10);
	}
}

void delay(char segundos){
	TPM2SC = 0b0001111;
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
	if (horas > 0 && horas <= HORAS_BAJO)
		return BAJO;
	if (horas > HORAS_BAJO && horas <= HORAS_MEDIO)
		return MEDIO;
	if (horas > HORAS_MEDIO)
		return ALTO;
}

