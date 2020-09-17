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

char determinarUso(int horas) {
	if (horas > 0 && horas <= HORAS_BAJO)
		return BAJO;
	if (horas > HORAS_BAJO && horas <= HORAS_MEDIO)
		return MEDIO;
	if (horas > HORAS_MEDIO)
		return ALTO;
}

