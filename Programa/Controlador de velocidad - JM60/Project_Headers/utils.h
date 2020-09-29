/*
 * utils.h
 *
 *  Created on: Sep 17, 2020
 *      Author: Pablo
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */

#define _DATA PTCDD_PTCDD0
#define DATA PTCD_PTCD0
#define _CLK PTCDD_PTCDD1
#define CLK PTCD_PTCD1
#define _TR PTCDD_PTCDD3
#define TR PTCD_PTCD3

#define ENCENDIDO 1
#define APAGADO 0

#define ENTRADA 0
#define SALIDA 1

#define HORAS_BAJO 20
#define HORAS_MEDIO 21

enum{
	OFF = 0,
	BAJO = 1,
	MEDIO = 2,
	ALTO = 3
};

char* my_itoa(int);
char cicloCompleto(char);
void mostrarNumero(unsigned char);
void mostrarHorasEnDisplay(int);
char determinarUso(int);
void delay(char);

#endif /* UTILS_H_ */
