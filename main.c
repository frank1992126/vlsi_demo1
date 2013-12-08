/*************************************************
 *  Author			: Frank Chang			 	 *
 *  Last Modified	: 2013/12/08			 	 *
 *  Description		: VLSI Combination Lock	Demo *
 *************************************************/

#include <stdio.h>
#include <stdlib.h>
#define boolean unsigned char

/* Function Prototypes */
void dumpPin();
int pin2Dec(boolean*,int);

/* Pin Declaration */
boolean PIN_IN_K[4];			// Key input [3:0]
boolean PIN_IN_CK[2];			// Current key position
boolean PIN_INOUT_S;			// Current State
boolean PIN_OUT_UL;				// Unlocked

/* Register Declaration */


void dumpPin() {
	printf("==== [Current Pin State] ====\n");
	printf("PIN_IN_N:\t0b%1d%1d%1d%1d\t(%x)\n", PIN_IN_K[3],
												 PIN_IN_K[2],
												 PIN_IN_K[1],
												 PIN_IN_K[0],
												 pin2Dec(PIN_IN_K, 4));
	printf("PIN_IN_CL:\t0b%1d%1d\t(%d)\n", PIN_IN_CK[1], PIN_IN_CK[0], pin2Dec(PIN_IN_CK, 2));
	printf("PIN_INOUT_S:\t0b%1d\n", PIN_INOUT_S);
	printf("PIN_OUT_UL:\t0b%1d\n", PIN_OUT_UL);
}

int pin2Dec(boolean* pins, int length) {
	int acc = 0;
	int i;

	for (i = 0; i < length; i++) {
		acc += (pins[i] << i); 
	}

	return acc;
}

int main(int argc, char** argv) {
	PIN_IN_K[3] = 1;
	PIN_IN_K[2] = 1;
	PIN_IN_K[1] = 1;
	PIN_IN_K[0] = 1;

	dumpPin();

}
