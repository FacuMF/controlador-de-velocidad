#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include "main.h"
#include <stdlib.h>

enum {
	cambioDeModo, modoNormal, modoServicio
};

enum {
	primeraMedicion, segundaMedicion, terceraMedicion
};

int medicion[83] = { 0 }, medicionFiltrada[41] = { 0 };
int t = 0, u = 0, contadorPulsos = 0;
char decodificacion = 0;
char esperoSigInstruccion = TRUE;

char confirmacionPendiente = 0;

int velElegida = 1;

char usoActual = BAJO;
unsigned char controlEncendido = 0;

char contadorSegundos = 0;
char contadorMinutos = 0;
int cantidadHoras = 0;
char j, index;

unsigned char digito[10] = { 0b11111100, 0b01100000, 0b11011010, 0b11110010,
		0b01100110, 0b10110110, 0b10111110, 0b11100000, 0b11111110, 0b11110110 };
char estado = modoNormal, estadoAnterior = modoServicio;

char timeOutServicio = 0;

int timeOutMotor = 0, tiempoMotor = 0;
char encenderMotor = 0;

char pOnOff = 0, pCV = 0, auxOnOff = 0, auxCV = 0;

char puntoInicial = 0, puntoFinal = 0;
int direccion = 0, valor = 0;

int resultado = 0, medicionEstadoBajo = 0;
char medicionControlCompleta = 0;
int loop;
char minutoCumplido = 0;
char initialCount = 0, energizado = 0;
void main(void) {
	char estadoOnOff = 0;

	char estadoCV = 0;
	SOPT1 &= 0x3F;
	EnableInterrupts
	;

	TPM1SC = 0b00000010; //divisor de 4 sin interrupciones
	TPM1SC_CLKSB = 0;
	TPM1SC_CLKSA = 1;
	TPM1C0SC = 0b01000000;//cambio a no deteccion

	initClock();
	initTimer();
	inicializacionPuertos();
	inicializacionPinInterrupts();
	RTC_Init();
	contadorMinutos = 0;

	timeOutMotor = getTimeOutMotor();

	controlEncendido = leer_byte(9);
	controlEncendido = leer_byte(9);

	velElegida = leer_byte(10);
	velElegida = leer_byte(10);
	puntoFinal = velElegida;
	if (controlEncendido)
		encender();

	
	for (;;) {
		if (minutoCumplido) {
			escribir_byte(9, contadorMinutos);
			escribir_byte(9, contadorMinutos);
			escribirHorasEnMemoria(cantidadHoras, PRIMARIA);
			minutoCumplido = 0;
		}
		if (medicionControlCompleta == 1) {
			medicionControlCompleta = 0;
			//if (medicion[0] >= 15900 && medicion[1] >= 7500) {
			//Filtro array
			for (j = 0; j <= 82; j++) {
				if (j % 2) {
					medicionFiltrada[index] = medicion[j];
					index++;
				}
			}
			index = 0;

			for (j = 0; j <= 41; j++) {
				if (medicionFiltrada[j] < 2000)
					medicionFiltrada[j] = 0;
				else {
					medicionFiltrada[j] = 1;
				}
			}
			for (loop = 0; loop < 20000; loop++)
				;

			if (medicionFiltrada[35] && medicionFiltrada[36]
					&& !medicionFiltrada[37] && !medicionFiltrada[38]
					&& !medicionFiltrada[39] && !medicionFiltrada[40]) {

				if (!medicionFiltrada[1] && !medicionFiltrada[2]
						&& medicionFiltrada[3] && !medicionFiltrada[4]
						&& controlEncendido)
					apagar();
				if (!medicionFiltrada[1] && !medicionFiltrada[2]
						&& medicionFiltrada[3] && medicionFiltrada[4]
						&& !controlEncendido)
					encender();
				else {
					//FUNCIONA
					if (medicionFiltrada[4] && controlEncendido) {
						puntoInicial = puntoFinal;
						puntoFinal = (medicionFiltrada[9])
								+ (medicionFiltrada[10]) * 2
								+ (medicionFiltrada[11]) * 4
								+ (medicionFiltrada[12]) * 8 - 5;

						direccion = puntoFinal - puntoInicial;

						if (direccion > 0)
							valor = 1;
						else if (direccion < 0)
							valor = -1;

						velElegida += valor;

						if (puntoFinal == 2 && puntoInicial == 1)
							velElegida = 1;
						if (puntoFinal == 8 && puntoInicial == 9)
							velElegida = 9;
						if (velElegida <= 1)
							velElegida = 1;
						if (velElegida >= 9)
							velElegida = 9;

						puntoInicial = velElegida;
						escribir_byte(10, velElegida);

						timeOutMotor = getTimeOutMotor();
						//if (velElegida != puntoInicial)
						notificarConBuzzer();

					}
				}

			}
			//}

			TPM1C0SC_ELS0B = 1;
			TPM1C0SC_ELS0A = 0;
		}
		if (ON_OFF == 0 && CV == 0)
			estado = cambioDeModo;

		switch (estado) {
		case cambioDeModo:
			estado = iniciarCambioDeModo(estadoAnterior);
			break;
		case modoNormal:
			estadoAnterior = estado;
			estadoOnOff = estadoPOnOff(&pOnOff, &auxOnOff);
			estadoCV = estadoPCV(&pCV, &auxCV);
			if (estadoOnOff) {
				(controlEncendido) ? apagar() : encender();
			}
			estadoOnOff = 0;

			if (controlEncendido) {
				indicarVelocidadElegida(velElegida);
				usoActual = determinarUso(cantidadHoras);
				mostrarUso(usoActual);
				if (estadoCV) {
					cambiarVelocidad();
				}
			} else {
				mostrarNumero(0);
				OPTO = 0;
				mostrarUso(APAGADO);
			}
			break;
		case modoServicio:
			estadoAnterior = estado;
			estadoOnOff = estadoPOnOff(&pOnOff, &auxOnOff);
			estadoCV = estadoPCV(&pCV, &auxCV);

			if (timeOutServicio >= TIME_OUT) {
				indicarCambioDeModo();
				estado = modoNormal;
				timeOutServicio = 0;
			}

			if (confirmacionPendiente) {
				mostrarNumero(CONFIRMACION);
				if (estadoOnOff == 1) {
					reiniciarHoras();
					confirmacionPendiente = 0;
					notificarConBuzzer();
				}
				if (estadoCV) {
					confirmacionPendiente = 0;
					notificarConBuzzer();
				}
			} else {
				mostrarNumero(GUION); //Muestro un giuon que indica que me encuentro en modo servicio
				if (estadoOnOff == 1) {
					confirmacionPendiente = 1;
					notificarConBuzzer();
				}

				if (estadoCV) {
					mostrarHorasEnDisplay(cantidadHoras);
				}

			}
			break;
		default:
			estado = modoNormal;
		}
	}
}

int getTimeOutMotor() {
	return -6 * velElegida + 83;
}

void initTimer(void) {
	/*TPM1SC = 0b01001000;
	 TPM1MOD = 799; //100us*/
	MTIMSC = 0b01000000;
	MTIMCLK = 0b00000010;
	MTIMMOD = 200;
}

void initClock() {
	/* ICSC1: CLKS=0,RDIV=0,IREFS=1,IRCLKEN=1,IREFSTEN=0 */
	ICSC1 = 0x06U; /* Initialization of the ICS control register 1 */
	/* ICSC2: BDIV=1,RANGE=0,HGO=0,LP=0,EREFS=1,ERCLKEN=1,EREFSTEN=0 */ICSC2 =
			0x46U; /* Initialization of the ICS control register 2 */
	while (ICSSC_IREFST == 0U) { /* Wait until the source of reference clock is internal clock */
	}
}

void RTC_Init(void) {
	/* RTCMOD: RTCMOD=0x1F */
	RTCMOD = 0x1FU; /* Set modulo register */
	/* RTCSC: RTIF=1,RTCLKS=1,RTIE=1,RTCPS=1 */
	RTCSC = 0xB1U; /* Configure RTC */
}

void inicializacionPuertos() {
	_USO_BAJO = SALIDA;
	_USO_MEDIO = SALIDA;
	_USO_ALTO = SALIDA;
	_OPTO = SALIDA;
	_BUZZER = SALIDA;
	_TR = SALIDA;
	_DATA = SALIDA;
	_CLK = SALIDA;
	USO_BAJO = APAGADO;
	USO_MEDIO = APAGADO;
	USO_ALTO = APAGADO;
	OPTO = APAGADO;
	BUZZER = APAGADO;
	TR = APAGADO;
	DATA = APAGADO;
	CLK = APAGADO;

	_ON_OFF = ENTRADA;
	_CV = ENTRADA;
	_RX = ENTRADA;
	_DC0 = ENTRADA;
}

__interrupt 26 void mtimInt(void) {
	MTIMSC_TOF = 0;

	if (controlEncendido) {
		if (encenderMotor == 1 && tiempoMotor < timeOutMotor) {
			tiempoMotor++;
			OPTO = 0;
		}
		if (tiempoMotor >= timeOutMotor) {
			tiempoMotor = 0;
			encenderMotor = 0;
			OPTO = 1;
		}
	}
}

void inicializacionPinInterrupts() {
	/*1. Mask interrupts by clearing PTxIE in PTxSC.
	 2. Select the pin polarity by setting the appropriate PTxESn bits in PTxES.
	 3. If using internal pull-up/pull-down device, configure the associated pull enable bits in PTxPE.
	 4. Enable the interrupt pins by setting the appropriate PTxPSn bits in PTxPS.
	 5. Write to PTxACK in PTxSC to clear any false interrupts.
	 6. Set PTxIE in PTxSC to enable interrupts.*/

	PTASC_PTAIE = 0; //limpio flag de interrupciones
	PTAES_PTAES1 = 1; // Detecta flancos ascendentes

	PTAPS_PTAPS1 = 1; //Activo interrupciones para DC0
	PTASC = 0b00001110; //PTAACK = 1, PTAIE = 1, PTAMOD = 0

	//Rpull up memoria
	//PTAPE_PTAPE0 = 1;
	PTAPE_PTAPE2 = 1;
	PTAPE_PTAPE3 = 1;
	//PTAES_PTAES0 = 0;
	PTAES_PTAES2 = 0;
	PTAES_PTAES3 = 0;
}

__interrupt 5 void tovf(void) {
	if(energizado)
		tiempoDeMedicion();
	else {
		contadorPulsos = 0;
	}
	TPM1C0SC_CH0F = 0;
}

void tiempoDeMedicion(void) {
	switch (decodificacion) {
	case primeraMedicion:
		TPM1C0SC_ELS0B = 1;
		TPM1C0SC_ELS0A = 1;
		decodificacion = segundaMedicion;
		u = TPM1C0V;
		break;
	case segundaMedicion:
		t = TPM1C0V;
		decodificacion = terceraMedicion;
		if (t - u < 0)
			medicionEstadoBajo = (t - u) * -1;
		else
			medicionEstadoBajo = t - u;
		decodificado(medicionEstadoBajo);
		break;
	case terceraMedicion:
		u = TPM1C0V;
		decodificacion = segundaMedicion;
		if (t - u < 0)
			resultado = (t - u) * -1;
		else
			resultado = t - u;
		decodificado(resultado);
		break;
	default:
		decodificacion = segundaMedicion;
	}
}

void decodificado(int res) {
	medicion[contadorPulsos] = res;
	contadorPulsos++;

	if (contadorPulsos == 82) {
		TPM1C0SC_ELS0B = 0;
		TPM1C0SC_ELS0A = 0;
		contadorPulsos++;
	}
	if (contadorPulsos >= 83) {
		medicionControlCompleta = 1;

		decodificacion = primeraMedicion;
		contadorPulsos = 0;

	} else {
		medicionControlCompleta = 0;
	}
}

void estadoControl(char onOff) {
	(onOff) ? encender() : apagar();
}

char iniciarCambioDeModo(char estadoAnterior) {
	int i;
	char reinicioCompleto = 0;
	TPM2SC = 0b0001111;
	TPM2MOD = 62499; //1s
	for (i = 0; i < TIEMPO_REINICIO; i++) {
		while (ON_OFF == 0 && CV == 0 && TPM2SC_TOF == 0)
			;
		TPM2SC_TOF = 0;
		if (ON_OFF == 0 && CV == 0)
			reinicioCompleto++;
	}
	TPM2SC = 0;
	if (reinicioCompleto == TIEMPO_REINICIO) {
		reinicioCompleto = 0;
		indicarCambioDeModo();
		if (estadoAnterior == modoNormal) {
			auxOnOff = 0;
			auxCV = 0;
			pOnOff = 0;
			pCV = 0;
			return modoServicio;
		}
		if (estadoAnterior == modoServicio) {
			auxOnOff = 0;
			auxCV = 0;
			pOnOff = 0;
			pCV = 0;
			return modoNormal;
		}
	} else {
		reinicioCompleto = 0;
		auxOnOff = 0;
		auxCV = 0;
		pOnOff = 0;
		pCV = 0;
		return estadoAnterior;
	}
}

void indicarCambioDeModo() {
	int i;
	unsigned char a = 0b11111111;
	mostrarNumero(a);
	for (i = 0; i < 6; i++) {
		a = ~a;
		mostrarNumero(a);
		delay(5);
	}
}

void reiniciarHoras() {
	cantidadHoras = 0;
	escribirHorasEnMemoria(cantidadHoras, PRIMARIA);
	escribirHorasEnMemoria(cantidadHoras, SECUNDARIA);
	mostrarHorasEnDisplay(0);
}

void cambiarVelocidad() {
	(velElegida >= LIMITE_VELOCIDADES) ? velElegida = 1 : velElegida++;
	escribir_byte(10, velElegida);
	timeOutMotor = getTimeOutMotor();
	notificarConBuzzer();
}

void setVelocidad(char vel) {
	velElegida = vel;
}

void notificarConBuzzer(void) {
	int i;
	for (i = 0; i < 10000; i++)
		BUZZER = ENCENDIDO;
	BUZZER = APAGADO;
}

void mostrarUso(char estado) {
	switch (estado) {
	case BAJO:
		USO_BAJO = ENCENDIDO;
		USO_MEDIO = APAGADO;
		USO_ALTO = APAGADO;
		break;
	case MEDIO:
		USO_BAJO = ENCENDIDO;
		USO_MEDIO = ENCENDIDO;
		USO_ALTO = APAGADO;
		break;
	case ALTO:
		USO_BAJO = ENCENDIDO;
		USO_MEDIO = ENCENDIDO;
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
	default:
		*aux = 0;
		*pOnOff = 0;
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
	default:
		*aux = 0;
		*pCV = 0;
	}
	return *aux;
}

void indicarVelocidadElegida(unsigned char velElegida) {
	mostrarNumero(digito[velElegida]);
}

void apagar() {
	//TODO: ver cuando pruebe la recepcion del control remoto
	//apagarTimer();
	PTASC_PTAIE = 0;	//apago interrupciones del flancos
	OPTO = 0;
	//mostrarUso(APAGADO);
	usoActual = APAGADO;
	controlEncendido = 0;
	notificarConBuzzer();
	escribirEstadoEnMemoria(0);
	TR = APAGADO;
}

void apagarTimer() {
	TPM1SC = 0;
}

void encender() {
	PTASC_PTAIE = 1;	//prendo interrupciones del flancos
	cantidadHoras = leerHorasDeMemoria();
	contadorMinutos = leer_byte(9);
	contadorMinutos = leer_byte(9);
	controlEncendido = 1;
	notificarConBuzzer();
	escribirEstadoEnMemoria(1);
	TR = ENCENDIDO;
}

void escribirEstadoEnMemoria(char estado) {
	escribir_byte(9, estado);
}

int leerHorasDeMemoria() {
	int horasLeidas;
	unsigned char horas[4];
	char esCorrecta;
	int i;
	leer_memo(horas, 0, 4);
	horasLeidas = horas[0] * 1000 + horas[1] * 100 + horas[2] * 10 + horas[3];
	esCorrecta = chequeoDeIntegridad(horasLeidas);
	if (!esCorrecta) {
		leer_memo(horas, SECUNDARIA, 4);
		horasLeidas = horas[0] * 1000 + horas[1] * 100 + horas[2] * 10
				+ horas[3];
		for (i = 0; i < 20000; i++)
			;
		escribirHorasEnMemoria(horasLeidas, PRIMARIA);
	}
	return horasLeidas;

}

char chequeoDeIntegridad(int horasLeidas) {
	int horasDeResguardo;
	unsigned char horas[4];
	int y1 = 0, y2 = 0;
	leer_memo(horas, SECUNDARIA, 4);
	horasDeResguardo = horas[0] * 1000 + horas[1] * 100 + horas[2] * 10
			+ horas[3];
	y1 = fHash(horasLeidas);
	y2 = fHash(horasDeResguardo);
	return y1 == y2;
}

int fHash(int x) {
	return 3 * x + 5;
}

__interrupt 20 void pinInterrupt() {
	PTASC_PTAACK = 1;
	encenderMotor = 1;
}

__interrupt 11 void tpmInt(void) {
	TPM1SC_TOF = 0;

	if (controlEncendido) {
		if (encenderMotor == 1 && tiempoMotor < timeOutMotor) {
			tiempoMotor++;
			OPTO = 0;
		}
		if (tiempoMotor >= timeOutMotor) {
			tiempoMotor = 0;
			encenderMotor = 0;
			OPTO = 1;
		}
	}

}

__interrupt 25 void rtcInt(void) {
	RTCSC_RTIF = 1;
	
	if(initialCount == 2){
		energizado = 1;
		contadorPulsos = 0;
		initialCount = 3;
	}
	if(initialCount == 1){
		TPM1C0SC_ELS0B = 1;
		TPM1C0SC_ELS0A = 0;
		//energizado = 1;
		initialCount = 2;
	}
	else
		initialCount++;
	
	if (estado == modoServicio) {
		timeOutServicio++;
	}

	if (controlEncendido) {
		contadorSegundos++;
	}
	if (contadorSegundos >= 60) {
		contadorSegundos = 0;
		contadorMinutos++;
		minutoCumplido = 1;
	}

	if (contadorMinutos == 30)
		escribirHorasEnMemoria(cantidadHoras, SECUNDARIA);

	if (contadorMinutos >= 60) {
		contadorMinutos = 0;
		contadorSegundos = 0;
		cantidadHoras++;
	}
}

void escribirHorasEnMemoria(int horas, int posicion) {
	unsigned char horasStr[4];

	horasStr[0] = horas / 1000;
	horasStr[1] = horas / 100 - horasStr[0] * 10;
	horasStr[2] = horas / 10 - horasStr[0] * 100 - horasStr[1] * 10;
	horasStr[3] = horas - horasStr[0] * 1000 - horasStr[1] * 100
			- horasStr[2] * 10;

	escribir_memo(horasStr, posicion, 4);
}

void mostrarNumero(unsigned char digitoMostrado) {
	int i;
	_DATA = SALIDA;
	_CLK = SALIDA;
	DATA = APAGADO;
	CLK = APAGADO;

	for (i = 0; i < 8; i++) {
		DATA = digitoMostrado & 1;
		CLK = 1;
		CLK = 0;
		digitoMostrado = digitoMostrado >> 1;
	}
	for (i = 0; i < 20000; i++)
		;
	for (i = 0; i < 20000; i++)
		;
	for (i = 0; i < 20000; i++)
		;
}

void mostrarHorasEnDisplay(int horas) {
	int horasAMostrar[4];
	char i;
	int j;
	for (i = 0; i < 4; i++) {
		horasAMostrar[3 - i] = horas % 10;
		horas /= 10;
	}

	for (i = 0; i < 4; i++) {
		notificarConBuzzer();
		mostrarNumero(digito[horasAMostrar[i]]);
		delay(10);
		mostrarNumero(0b00000000);
		for (j = 0; j < 20000; j++)
			;
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
