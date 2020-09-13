/*
 * main.h
 *
 *  Created on: Aug 6, 2020
 *      Author: Usuario
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "i2c.h"

//SH4
/*#define _ON_OFF PTADD_PTADD2
#define ON_OFF PTAD_PTAD2
#define _CV PTADD_PTADD2
#define  CV PTAD_PTAD2

#define _LED_INDICADOR PTCDD_PTCDD1
#define LED_INDICADOR PTCD_PTCD1

#define _RX PTADD_PTADD0
#define RX PTAD_PTAD0
#define _DC0 PTADD_PTADD1
#define DC0 PTAD_PTAD1

#define _USO_BAJO PTBDD_PTBDD2
#define USO_BAJO PTBD_PTBD2
#define _USO_MEDIO PTBDD_PTBDD3
#define USO_MEDIO PTBD_PTBD3
#define _USO_ALTO PTBDD_PTBDD4
#define USO_ALTO PTBD_PTBD4
#define _OPTO PTBDD_PTBDD5
#define OPTO PTBD_PTBD5*/

#define _ON_OFF PTGDD_PTGDD0
#define ON_OFF PTGD_PTGD0

#define _CV PTCDD_PTCDD5
#define  CV PTCD_PTCD5

#define _LED_INDICADOR PTCDD_PTCDD1
#define LED_INDICADOR PTCD_PTCD1

#define _RX PTADD_PTADD0
#define RX PTAD_PTAD0
#define _DC0 PTADD_PTADD1
#define DC0 PTAD_PTAD1

#define _USO_BAJO PTBDD_PTBDD0
#define USO_BAJO PTBD_PTBD0
#define _USO_MEDIO PTBDD_PTBDD1
#define USO_MEDIO PTBD_PTBD1
#define _USO_ALTO PTBDD_PTBDD2
#define USO_ALTO PTBD_PTBD2
#define _OPTO PTBDD_PTBDD3
#define OPTO PTBD_PTBD3


#define IntDC0 KBIES_KBEDG0
#define IntRX KBIES_KBEDG1

#define LIMITE_VELOCIDADES 10

#define HORAS_BAJO 2500
#define HORAS_MEDIO 5000

#define TIEMPO_REINICIO 3

#define ENCENDIDO 1
#define APAGADO 0

#define ENTRADA 0
#define SALIDA 1

#define ABIERTO 1
#define PULSADO 0

enum{
	VEL_1 = 1,
	VEL_2  = 2,
	VEL_3 = 3,
	VEL_4 = 4,
	VEL_5 = 5,
	VEL_6 = 6,
	VEL_7 = 7,
	VEL_8 = 8,
	VEL_9 = 9,
	VEL_10 = 10,
	
};


enum{
	OFF = 0,
	BAJO = 1,
	MEDIO = 2,
	ALTO = 3
};

enum {
	primeraMedicion, segundaMedicion
};

void inicializacionPuertos(void);
void inicializacionPinInterrupts(void);
void inicializacionTimer(void);
void initDeteccionFlancos(void);

void tiempoDeMedicion(void);
void decodificado(int);

void mostrarUso(char);
char estadoPOnOff(char*, char*);
char estadoPCV(char*, char*);
void indicarVelocidadElegida(void);
void titilar(int);
void cambiarVelocidad(void);

void manejarDC0(void);
void manejarRX(void);

void apagar(void);
void apagarTimer(void);
void encender(void);
void reiniciar(void);
void reiniciarHoras(void);
void indicarReinicio(void);

void escribirHorasEnMemoria(int);
int leerHorasDeMemoria(void);
void determinarUso(int);

void leerCodigo(char*);
void leerEstado(char*);
void leerVelocidad(char*);

char* my_itoa(int);
char cicloCompleto(void);


#endif /* MAIN_H_ */
