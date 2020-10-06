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
char confirmacionPendiente = 0;

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

unsigned char digito[10] = { 0b01111110, 0b00110000, 0b01101101, 0b01111001,
		0b00110011, 0b01011011, 0b01011111, 0b01110000, 0b01111111, 0b01111011 };
char estado = modoNormal, estadoAnterior = modoServicio;
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

			if (controlEncendido) {
				indicarVelocidadElegida(velElegida);
				usoActual = determinarUso(cantidadHoras);
				//TODO: ver que pasa si alcanzo las 5000 horas
				mostrarUso(usoActual);
				if (estadoCV) {
					cambiarVelocidad();
				}
			}
			else
				mostrarNumero(0);
			break;
		case modoServicio:
			estadoAnterior = estado;
			estadoOnOff = estadoPOnOff(&pOnOff, &auxOnOff);
			estadoCV = estadoPCV(&pCV, &auxCV);
			if(confirmacionPendiente){
				mostrarNumero(CONFIRMACION);
				if (estadoOnOff == 1){
					reiniciarHoras();
					confirmacionPendiente = 0;
					notificarConBuzzer();
				}
				if(estadoCV){
					confirmacionPendiente = 0;
					notificarConBuzzer();
				}
			}
			else{
				mostrarNumero(GUION);//Muestro un giuon que indica que me encuentro en modo servicio
				if (estadoOnOff == 1){
					confirmacionPendiente = 1;
					notificarConBuzzer();
				}
	
				if (estadoCV){
					mostrarHorasEnDisplay(cantidadHoras);
					notificarConBuzzer();
				}
				
			}
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
	_BUZZER = SALIDA;
	_TR = SALIDA;
	_DATA = SALIDA;
	_CLK = SALIDA;

	USO_BAJO = APAGADO;
	USO_MEDIO = APAGADO;
	USO_ALTO = APAGADO;
	OPTO = APAGADO;
	BUZZER = APAGADO;
	TR = ENCENDIDO;
	DATA = APAGADO;
	CLK = APAGADO;
	
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
		if (estadoAnterior == modoNormal)
			return modoServicio;
		if (estadoAnterior == modoServicio)
			return modoNormal;
	} else{
		reinicioCompleto = 0;
		return modoNormal;
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
	escribirHorasEnMemoria(0, PRIMARIA);
	escribirHorasEnMemoria(0, SECUNDARIA);
	mostrarHorasEnDisplay(0);
}

void cambiarVelocidad() {
	(velElegida == LIMITE_VELOCIDADES) ? velElegida = VEL_1 : velElegida++;
	notificarConBuzzer();	
}

void setVelocidad(char vel) {
	velElegida = vel + 1;
}

void notificarConBuzzer(void){
	int i;	
	for(i = 0; i<10000; i++)
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

void indicarVelocidadElegida(unsigned char velElegida) {
	mostrarNumero(digito[velElegida]);
}

void apagar() {
	apagarTimer();
	mostrarUso(APAGADO);
	controlEncendido = 0;
	notificarConBuzzer();
}

void apagarTimer() {
	TPM1SC = 0;
}

void encender() {
	inicializacionTimer();
	cantidadHoras = leerHorasDeMemoria();
	controlEncendido = 1;
	notificarConBuzzer();
}

int leerHorasDeMemoria() {
	int horasLeidas;
	unsigned char horas[4];
	char esCorrecta;
	leer_memo(horas, 0, 4);
	horasLeidas = atoi(horas);
	esCorrecta = chequeoDeIntegridad(horasLeidas);
	if (!esCorrecta) {
		leer_memo(horas, 8, 4);
		horasLeidas = atoi(horas);
		escribirHorasEnMemoria(horasLeidas, PRIMARIA);
	}
	return horasLeidas;

}

char chequeoDeIntegridad(int horasLeidas) {
	int horasDeResguardo;
	unsigned char horas[4];
	int y1 = 0, y2 = 0;
	leer_memo(horas, 8, 4);
	horasDeResguardo = atoi(horas);
	y1 = fHash(horasLeidas);
	y2 = fHash(horasDeResguardo);
	return y1 == y2;
}

int fHash(int x){
	return 3*x+5;
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
		contadorMinutos = 0; 
		contadorSegundos = 0;
		cantidadHoras++;
		escribirHorasEnMemoria(cantidadHoras, PRIMARIA);
		//escribirHorasEnMemoria(cantidadHoras, SECUNDARIA);
		//TODO: revisar
	}
}

void escribirHorasEnMemoria(int horas, int posicion) {
	char i;
	unsigned char horasStr[4];
	int cantidadARellenar;
	sprintf(horasStr, "%d", horas);
	cantidadARellenar = 4 - strlen(horasStr);
	for (i = 0; i < cantidadARellenar; i++)
		horasStr[3 - i] = '0';
	escribir_memo(horasStr, posicion, strlen(horasStr));
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

void mostrarNumero(unsigned char digitoMostrado){
	int i;
	int numero;
	_DATA = SALIDA;
	_CLK = SALIDA;
	DATA = APAGADO;
	CLK = APAGADO;
	
	for (i = 0; i < 7; i++) {
		numero = digitoMostrado & 1; 
		DATA = numero;
		CLK = 1;
		CLK = 0;
		digitoMostrado = digitoMostrado >> 1;
	}
	for(i=0;i<20000;i++);
}

void mostrarHorasEnDisplay(int horas){
	int horasAMostrar[4];
	char i;
	horasAMostrar[0] = horas/1000;
	horasAMostrar[1] = horas/100 - horasAMostrar[0]*10;
	horasAMostrar[2] = horas/10 - horasAMostrar[0]*100 - horasAMostrar[1]*10;
	horasAMostrar[3] = horas - horasAMostrar[0]*1000 - horasAMostrar[1]*100 - horasAMostrar[2]*10;
	
	for(i = 0; i<4; i++){
		mostrarNumero(digito[horasAMostrar[i]]);
		delay(10);
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
