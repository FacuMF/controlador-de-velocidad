/*
 * decodificacion.c
 *
 *  Created on: Sep 17, 2020
 *      Author: Pablo
 */
#include "decodificacion.h"

char leerEstado(char* codigo, char estadoActual) {
	char res;
	if (codigo[3] == '0' && estadoActual == '1') {
		res = 0;
		estadoActual = '0';
	}
	if (codigo[3] == '1' && estadoActual == '0') {
		res = 1;
		estadoActual = '1';
	}
	return res;
}

char leerVelocidad(char* codigo) {
	char temperatura[4];
	char numero;
	temperatura[0] = codigo[11];
	temperatura[1] = codigo[10];
	temperatura[2] = codigo[9];
	temperatura[3] = codigo[8];
	numero = atoi(temperatura) - 19; //20° a 29°
	return numero;
}
