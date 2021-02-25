/*
 * main.h
 *
 *  Created on: Aug 6, 2020
 *      Author: Usuario
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "i2c.h"
#include "utils.h"

//SH4
#define _ON_OFF PTBDD_PTBDD1
#define ON_OFF PTBD_PTBD1
#define _CV PTBDD_PTBDD0
#define  CV PTBD_PTBD0

#define _DATA PTCDD_PTCDD0
#define DATA PTCD_PTCD0
#define _CLK PTCDD_PTCDD1
#define CLK PTCD_PTCD1

#define _RX PTADD_PTADD0
#define RX PTAD_PTAD0
#define _DC0 PTADD_PTADD1
#define DC0 PTAD_PTAD1

#define _USO_BAJO PTBDD_PTBDD4
#define USO_BAJO PTBD_PTBD4
#define _USO_MEDIO PTBDD_PTBDD3
#define USO_MEDIO PTBD_PTBD3
#define _USO_ALTO PTBDD_PTBDD2
#define USO_ALTO PTBD_PTBD2
#define _OPTO PTBDD_PTBDD5
#define OPTO PTBD_PTBD5

#define _BUZZER PTCDD_PTCDD2
#define BUZZER PTCD_PTCD2
#define _TR PTCDD_PTCDD3
#define TR PTCD_PTCD3

/*#define _ON_OFF PTGDD_PTGDD0
#define ON_OFF PTGD_PTGD0

#define _CV PTCDD_PTCDD5
#define  CV PTCD_PTCD5

#define _DATA PTCDD_PTCDD0
#define DATA PTCD_PTCD0
#define _CLK PTCDD_PTCDD1
#define CLK PTCD_PTCD1

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

#define _BUZZER PTGDD_PTGDD3
#define BUZZER PTGD_PTGD3

#define _TR PTCDD_PTCDD3
#define TR PTCD_PTCD3*/

#define IntDC0 KBIES_KBEDG0
#define IntRX KBIES_KBEDG1

#define LIMITE_VELOCIDADES 9

#define TIEMPO_REINICIO 3

#define ENCENDIDO 1
#define APAGADO 0

#define ENTRADA 0
#define SALIDA 1

#define ABIERTO 1
#define PULSADO 0

#define VACIO 0b10011110
#define GUION 0b00000010
#define CONFIRMACION 0b10011100

#define PRIMARIA 0
#define SECUNDARIA 4

#define TIME_OUT 30


void inicializacionPuertos(void);
void inicializacionPinInterrupts(void);
void inicializacionTimer(void);
void initDeteccionFlancos(void);
void initClock(void);
void RTC_Init(void);
void initTimer(void);

void tiempoDeMedicion(void);
void decodificado(int);

void mostrarUso(char);
char estadoPOnOff(char*, char*);
char estadoPCV(char*, char*);
void indicarVelocidadElegida(unsigned char);
void cambiarVelocidad(void);

void manejarDC0(void);
void manejarRX(void);

void apagar(void);
void apagarTimer(void);
void encender(void);

char iniciarCambioDeModo(char);
void reiniciarHoras(void);
void indicarCambioDeModo(void);

void escribirHorasEnMemoria(int, int);
int leerHorasDeMemoria(void);
void leerSecuencia(char);

void notificarConBuzzer(void);

char chequeoDeIntegridad(int);
int fHash(int);

void estadoControl(char);
void setVelocidad(char);

void mostrarNumero(unsigned char);
void mostrarHorasEnDisplay(int);

void escribirEstadoEnMemoria(char);
unsigned char leerEstadoEnMemoria(void);

int getTimeOutMotor(void);

#endif /* MAIN_H_ */
