#include <hidef.h> /* for EnableInterrupts macro */
#include "derivative.h" /* include peripheral declarations */
#include <string.h>

#define _LED1 PTCDD_PTCDD2
#define LED1 PTCD_PTCD2
#define _SALIDA PTCDD_PTCDD1
#define SALIDA PTCD_PTCD1
#define _PULSADOR PTGDD_PTGDD0
#define PULSADOR PTGD_PTGD0

#define _irIn PTFDD_PTFDD0
#define irIn PTFD_PTFD0

#define HIGH 1
#define LOW 0
#define TRUE 1
#define FALSE 0

#define PULSADO 0
#define ABIERTO 1

int t = 0, u = 0, w = 0, i = 0, j = 0;
int bitDeco = 0, decodificacion = 0;
char v[40] = { 0 }, esperoSigInstruccion = FALSE;
char confirmacion = 0;

char START = TRUE, terminoStart = 0;
char traduccion[11] = { 0 };
int resultado;

void SCI_INIT(void);
void tiempoDeMedicion(void);
void decodificado(int);
void enviarCodigo(char[]);
void conversionDeCodigo(char codigo[]);
char binarioAHexa(char nibble[]);
enum {
	primeraMedicion, segundaMedicion
};
int estado_p = 0, p;
enum {
	Pul_ap, Pul_en, sostener
};
int me_pulsador(void);

char prueba[41] = "0000010010110010010110010010100101100011";
char aux[41];
void main(void) {
	SOPT1 &= 0x3F;
	EnableInterrupts
	;
	SCI_INIT();
	TPM1SC = 0b00000010; //divisor de 4 sin interrupciones
	TPM1C2SC = 0b01001100; //configurtado para flancos ascendentes 
	TPM1SC_CLKSB = 0;
	TPM1SC_CLKSA = 1;
	_SALIDA = 1;
	_LED1 = 1;
	LED1 = 0;
	_irIn = 0;

	TPM2SC = 0b00001010;
	TPM2MOD = 1999;
	for (;;) {
		
	}
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
		t = TPM1C2V;
		decodificacion = segundaMedicion;
		break;
	case segundaMedicion:
		u = TPM1C2V;
		decodificacion = primeraMedicion;
		if (w == 48) {
			w = 0;
			LED1 = HIGH;
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
	if (res > 800 && res <= 1300) {
		v[i] = '0';
		i++;
	}
	if (res > 2900 && res <= 3500) {
		v[i] = '1';
		i++;
	}
	if (i == 40) {
		terminoStart = 0;
		esperoSigInstruccion = TRUE;
		TPM1C2SC_ELS2B = 0;
		TPM1C2SC_ELS2A = 1;
		i = 0;
		conversionDeCodigo(v);
		strcat(v,"\n");
		traduccion[10] = '\n';
		traduccion[11] = '\0';
		enviarCodigo(v);
	}
}

void enviarCodigo(char codigo[]) {
	int i = 0;
	for (i = 0; codigo[i] != '\0'; i++) {
		SCI1D = codigo[i];
		while (!SCI1S1_TDRE)
			;
	}
}
void SCI_INIT(void) {
	SCI1BDL = 52; // divisor = 52 baud = 8MHz / 52 / 16 = 9615,38 ? 9600
	SCI1C1 = 0b00000000; // paridad habilitada – paridad par
	SCI1C2 = 0b00101100; // habilita interrupción de recepción, transmisor y receptor
	SCI1C3 = 0;
}
//comparar los primeros 4 bits y despues desplazar para comparar otros 4 
//hacer varios if como pasar de binario a hexa

void conversionDeCodigo(char codigo[]) {
	char aux[41];
	int h = 0, i = 0, j = 0, k = 0, l = 0, m = 0;
	char nibble[5] = {0};
	strcpy(aux,codigo);
	for (k = 0; k < 10; k++) {
		for (h = 0; h < 4; h++) {
			nibble[h] = aux[h];
		}
		nibble[4] = '\0';
		if (j == 10)
			j = 0;
		else {
			traduccion[j] = binarioAHexa(nibble);
			j++;
		}
		for (l = 0; l < 4; l++) {
			for (m = 0; m < 40; m++)
				aux[m] = aux[m + 1];
		}
	}
	confirmacion = 1;
}
char binarioAHexa(char nib[]) {
	if (strcmp(nib, "0000") == 0)
		return '0';
	if (strcmp(nib, "0001") == 0)
		return '1';
	if (strcmp(nib, "0010") == 0)
		return '2';
	if (strcmp(nib, "0011") == 0)
		return '3';
	if (strcmp(nib, "0100") == 0)
		return '4';
	if (strcmp(nib, "0101") == 0)
		return '5';
	if (strcmp(nib, "0110") == 0)
		return '6';
	if (strcmp(nib, "0111") == 0)
		return '7';
	if (strcmp(nib, "1000") == 0)
		return '8';
	if (strcmp(nib, "1001") == 0)
		return '9';
	if (strcmp(nib, "1010") == 0)
		return 'A';
	if (strcmp(nib, "1011") == 0)
		return 'B';
	if (strcmp(nib, "1100") == 0)
		return 'C';
	if (strcmp(nib, "1101") == 0)
		return 'D';
	if (strcmp(nib, "1110") == 0)
		return 'E';
	if (strcmp(nib, "1111") == 0)
		return 'F';
}
