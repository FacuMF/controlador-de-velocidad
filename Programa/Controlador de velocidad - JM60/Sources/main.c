#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "main.h"
#include <stdlib.h>
#include <string.h>

int t = 0, u = 0, w = 0, contadorPulsos = 0, j = 0;
int bitDeco = 0, decodificacion = 0;
char v[40] = { 0 }, esperoSigInstruccion = FALSE;
char confirmacion = 0;

char START = TRUE, terminoStart = 0;
char traduccion[11] = { 0 };
int resultado;
char prueba[41] = "0000010010110010010110010010100101100011";
char aux[41];

char estadoActual = '0';

char secuencia[10][20] = { { 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
		0, 1, 1 },
		{ 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1 }, { 1, 0,
				0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 1 }, { 1, 0,
				0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1 }, { 1, 0,
				0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1 }, { 1, 1,
				1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1 }, { 1, 1,
				1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1 }, { 1, 1,
				1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1 }, { 1, 1,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1 }, { 1, 1,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } };
char velElegida = VEL_1;

char usoActual = BAJO;
char controlEncendido = 0;
char contadorCiclos = 0;

char contadorSegundos = 0;
char contadorMinutos = 0;
int cantidadHoras = 0;
char pulsos = 0;

unsigned char digito[10] = { 0b11111100, 0b01100000, 0b11011010, 0b11110010,
		0b01100110, 0b10110110, 0b10111110, 0b11100000, 0b11111110, 0b11110110 };
char estado = modoNormal;
void main(void) {
	char estadoOnOff = 0;
	char estadoCV = 0;
	char pOnOff = 0, pCV = 0, auxOnOff = 0, auxCV = 0;
	SOPT1 &= 0x3F;
	EnableInterrupts
	;

	inicializacionPuertos();
	initDeteccionFlancos();
	//inicializacionPinInterrupts();

	//TODO: grabar 0s en la memoria para que empiece bien
	for (;;) {
		if (ON_OFF == 1 && CV == 1)
			estado = cambioDeModo;

		switch (estado) {
		case cambioDeModo:
			estado = iniciarCambioDeModo();
			break;
		case modoNormal:
			estadoOnOff = estadoPOnOff(&pOnOff, &auxOnOff);
			estadoCV = estadoPCV(&pCV, &auxCV);

			if (estadoOnOff) {
				(controlEncendido) ? apagar() : encender();
			}

			if (controlEncendido) {
				usoActual = determinarUso(cantidadHoras);
				mostrarUso(usoActual);
				if (estadoCV) {
					cambiarVelocidad();
					indicarVelocidadElegida(velElegida);
				}
			}
			break;
		case modoServicio:
			estadoOnOff = estadoPOnOff(&pOnOff, &auxOnOff);
			estadoCV = estadoPCV(&pCV, &auxCV);

			if (estadoOnOff)
				reiniciarHoras();

			if (estadoCV)
				mostrarHorasEnDisplay(cantidadHoras);
			break;
		default:
			estado = modoNormal;
		}

	}
}

void initDeteccionFlancos() {
	//TPM1SC = 0b00000010; //divisor de 4 sin interrupciones
	TPM1C1SC = 0b01001100; //configurtado para flancos ascendentes 
	TPM1SC_CLKSB = 0;
	TPM1SC_CLKSA = 1;
}

void inicializacionPuertos() {
	_USO_BAJO = SALIDA;
	_USO_MEDIO = SALIDA;
	_USO_ALTO = SALIDA;
	_OPTO = SALIDA;

	USO_BAJO = APAGADO;
	USO_MEDIO = APAGADO;
	USO_ALTO = APAGADO;
	OPTO = APAGADO;

	_ON_OFF = ENTRADA;
	_CV = ENTRADA;
	_RX = ENTRADA;
	_DC0 = ENTRADA;
}

void inicializacionTimer() {
	TPM1SC = 0b01001111;
	TPM1MOD = 62499; //Divisor: 128 => TOF cada 1s
}

void inicializacionPinInterrupts() {
	KBISC = 0; //reinicio
	IntDC0 = 1; //activo int por flag asc para DC0
	//TODO: ver como obtener señal de rx con el proyecto del receptor
	KBIPE = 0x03; //Interrupciones para G0 y G1 
	KBISC_KBACK = 0;
	KBISC = 0x02; //int habilitadas y solo detecta flancos asc y desc
}

__interrupt 11 void tovf(void) {
	if (esperoSigInstruccion == TRUE) {
		TPM1C2SC_ELS2B = 1;
		TPM1C2SC_ELS2A = 1;
		esperoSigInstruccion = FALSE;
	}
	tiempoDeMedicion();
	TPM1C2SC_CH2F = 0;
}
void tiempoDeMedicion(void) {
	w++;
	switch (decodificacion) {
	case primeraMedicion:
		t = TPM1C0V;
		decodificacion = segundaMedicion;
		break;
	case segundaMedicion:
		u = TPM1C0V;
		decodificacion = primeraMedicion;
		if (w == 48) {
			w = 0;
		}
		if (t - u < 0)
			resultado = (t - u) * -1;
		else
			resultado = t - u;
		decodificado(resultado);
		break;
	}
}

void decodificado(int res) {
	char onOff, velocidad;
	if (res > 800 && res <= 1300) {
		v[contadorPulsos] = '0';
		contadorPulsos++;
	}
	if (res > 2900 && res <= 3500) {
		v[contadorPulsos] = '1';
		contadorPulsos++;
	}
	if (contadorPulsos == 40) {
		terminoStart = 0;
		esperoSigInstruccion = TRUE;
		TPM1C0SC_ELS0B = 0;
		TPM1C0SC_ELS0A = 1;
		contadorPulsos = 0;
		onOff = leerEstado(v, estadoActual);
		velocidad = leerVelocidad(v);
		estadoControl(onOff);
		setVelocidad(velocidad);
	}
}

void estadoControl(char onOff) {
	(onOff) ? encender() : apagar();
}

char iniciarCambioDeModo() {
	int i;
	char reinicioCompleto = 0;
	TPM2SC = 0b0001111;
	TPM2MOD = 62499; //1s
	for (i = 0; i < TIEMPO_REINICIO; i++) {
		while (ON_OFF == 1 && CV == 1 && TPM2SC_TOF == 0)
			;
		TPM2SC_TOF = 0;
		if (ON_OFF == 1 && CV == 1)
			reinicioCompleto++;
	}
	TPM2SC = 0;
	if (reinicioCompleto == TIEMPO_REINICIO) {
		indicarCambioDeModo();
		if (estado == modoNormal)
			return modoServicio;
		if (estado == modoServicio)
			return modoNormal;
	} else
		return modoNormal;
}

void indicarCambioDeModo() {
	int i;
	mostrarNumero(VACIO);
	for (i = 0; i < 6; i++) {
		mostrarNumero(~VACIO);
		delay(5);
	}
}

void reiniciarHoras() {
	escribirHorasEnMemoria(0);
	mostrarHorasEnDisplay(0);
}

void cambiarVelocidad() {
	(velElegida == LIMITE_VELOCIDADES) ? velElegida = VEL_1 : velElegida++;
}

void setVelocidad(char vel) {
	velElegida = vel + 1;
}

void mostrarUso(char estado) {
	switch (estado) {
	case BAJO:
		USO_BAJO = ENCENDIDO;
		USO_MEDIO = APAGADO;
		USO_ALTO = APAGADO;
		break;
	case MEDIO:
		USO_BAJO = APAGADO;
		USO_MEDIO = ENCENDIDO;
		USO_ALTO = APAGADO;
		break;
	case ALTO:
		USO_BAJO = APAGADO;
		USO_MEDIO = APAGADO;
		USO_ALTO = ENCENDIDO;
		break;
	case OFF:
		USO_BAJO = APAGADO;
		USO_MEDIO = APAGADO;
		USO_ALTO = APAGADO;
		break;
	default:
		USO_BAJO = APAGADO;
		USO_MEDIO = APAGADO;
		USO_ALTO = APAGADO;
	}
}

char estadoPOnOff(char* pOnOff, char* aux) {
	switch (*pOnOff) {
	case 0:
		*aux = 0;
		if (ON_OFF == PULSADO)
			*pOnOff = 1;
		break;
	case 1:
		*aux = 1;
		if (ON_OFF == ABIERTO)
			*pOnOff = 0;
		if (ON_OFF == PULSADO)
			*pOnOff = 2;
		break;
	case 2:
		*aux = 0;
		if (ON_OFF == ABIERTO)
			*pOnOff = 0;
		break;
	}
	return *aux;
}

char estadoPCV(char* pCV, char* aux) {
	switch (*pCV) {
	case 0:
		*aux = 0;
		if (CV == PULSADO)
			*pCV = 1;
		break;
	case 1:
		*aux = 1;
		if (CV == ABIERTO)
			*pCV = 0;
		if (CV == PULSADO)
			*pCV = 2;
		break;
	case 2:
		*aux = 0;
		if (CV == ABIERTO)
			*pCV = 0;
		break;
	}
	return *aux;
}

void indicarVelocidadElegida(char velElegida) {
	mostrarNumero(digito[velElegida]);
}

void apagar() {
	apagarTimer();
	mostrarUso(APAGADO);
	controlEncendido = 0;
}

void apagarTimer() {
	TPM1SC = 0;
}

void encender() {
	inicializacionTimer();
	cantidadHoras = leerHorasDeMemoria();
	controlEncendido = 1;
}

int leerHorasDeMemoria() {
	int horasLeidas;
	unsigned char horas[4];
	char esCorrecta;
	leer_memo(horas, 0, 4);
	horasLeidas = atoi(horas);
	esCorrecta = chequeoIntegridad(horasLeidas);
	if (!esCorrecta) {
		leer_memo(horas, 8, 4);
		horasLeidas = atoi(horas);
	}
	return horasLeidas;

}

char chequeoDeIntegridad(int horas) {
	int horasDeResguardo;
	unsigned char horas[4];
	int y1 = 0, y2 = 0;
	leer_memo(horas, 8, 4);
	horasLeidas = atoi(horas);
	y1 = fHash(horas);
	y2 = fHash(horasDeResguardo);
	return y1 == y2;
}

int fHash(int x){
	return 3*x+3;
}

__interrupt 25 void pinInterrupt() {
	KBISC_KBACK = 0;
	manejarDC0();
}

__interrupt 15 void tm1Interrupt(void) {
	TPM1SC_TOF = 0;
	if (controlEncendido) {
		contadorSegundos++;
	}
	if (contadorSegundos == 60) {
		contadorSegundos = 0;
		contadorMinutos++;
	}

	if (contadorMinutos == 30)
		escribirHorasEnMemoria(cantidadHoras, SECUNDARIA);

	if (contadorMinutos == 60) {
		cantidadHoras++;
		escribirHorasEnMemoria(cantidadHoras, PRIMARIA);
		escribirHorasEnMemoria(cantidadHoras, SECUNDARIA);
		contadorMinutos = 0;
	}
}

void escribirHorasEnMemoria(int horas, int posicion) {
	char i;
	unsigned char horasStr[4];
	int cantidadARellenar;
	memcpy(horasStr, my_itoa(horas), 4);
	cantidadARellenar = 4 - strlen(horasStr);
	for (i = 0; i < cantidadARellenar; i++)
		horasStr[i] = '0';
	escribir_memo(horasStr, 0, strlen(horasStr));
}

void manejarDC0() {
	if (cicloCompleto(pulsos)) {
		if (contadorCiclos < 20) {
			OPTO = secuencia[velElegida][contadorCiclos];
			contadorCiclos++;
		} else
			contadorCiclos = 0;
	}
}

/*
 * DCO:
 * - Las interrupciones llegan cada 2 ms, teniendo en cuenta que un ciclo entero son 
 */

/*
 * Memoria
 * Primero debo intentar leer la memoria para ver si hay una hora guardada, si no hay horas(o sea que hay basura) debo escribir un 0 dado que no hay nada guardado.
 * Durante el funcionamiento normal voy a incrementar las horas y a medida que esto pase debo escrbit en la memoria(cada una hora escribo)
 */
