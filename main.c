/*************************************************
 *  Author			: Frank Chang			 	 *
 *  Last Modified	: 2013/12/08			 	 *
 *  Description		: VLSI Combination Lock	Demo *
 *************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char pin;
typedef struct registers {
  pin d[4];
} reg;
typedef unsigned char uint1;
typedef char int1;

/* Function Prototypes */
void dumpPin();
int pin2Dec(pin*,int);

/* Pin Declaration */
pin PIN_IN_K[4];			// Key input [3:0]
pin PIN_IN_CK[2];			// Current key position
pin PIN_OUT_UL;				// Unlocked
pin PIN_OUT_E5;				// 5 Errors
pin PIN_OUT_E3;				// 3 Errors

/* Register Declaration */
reg KEY[4];					// Current stored key
reg BUF[4];					// Input buffer
reg ERR[3];					// Error Counter

void dumpPin() {
	printf("==== [Current Pin State] ====\n");
	printf("PIN_IN_K:\t0b%1d%1d%1d%1d\t(%x)\n", PIN_IN_K[3],
												 PIN_IN_K[2],
												 PIN_IN_K[1],
												 PIN_IN_K[0],
												 pin2Dec(PIN_IN_K, 4));
	printf("PIN_IN_CK:\t0b%1d%1d\t(%d)\n", PIN_IN_CK[1], PIN_IN_CK[0], pin2Dec(PIN_IN_CK, 2));
	printf("PIN_OUT_UL:\t0b%1d\t[%s]\n", PIN_OUT_UL, PIN_OUT_UL?"UNLOCKED":"LOCKED");
}

void dumpReg() {
	printf("==== [Current Reg State] ====\n");
	printf("KEY:\t0x%x%x%x%x\n", pin2Dec(KEY[0].d, 4),
								 pin2Dec(KEY[1].d, 4),
								 pin2Dec(KEY[2].d, 4),
								 pin2Dec(KEY[3].d, 4));
	printf("BUF:\t0x%x%x%x%x\n", pin2Dec(BUF[0].d, 4),
								 pin2Dec(BUF[1].d, 4),
								 pin2Dec(BUF[2].d, 4),
								 pin2Dec(BUF[3].d, 4));
}

int pin2Dec(pin* pins, int length) {
	int acc = 0;
	int i;

	for (i = 0; i < length; i++) {
		acc += (pins[i] << i); 
	}

	return acc;
}

int regCmp(reg A, reg B, int length) {
	int i,match=1;
	for (i=0; i<length && match; i++) match = A.d[i] == B.d[i];
	return match;
}

void reset() {
	bzero(PIN_IN_K, sizeof(pin) * 4);
	bzero(PIN_IN_CK, sizeof(pin) * 2);
	bzero(KEY, sizeof(struct registers) * 4);
	bzero(BUF, sizeof(struct registers) * 4);
	PIN_OUT_UL = 1;
}

int main(int argc, char** argv) {
	uint1 input;					// user input
	uint1 curPos;				// current key pos
	uint1 errCount = 0;			// error counter
	uint1 bitpos=3;				// which bit
	
	/* Initialization */
	fprintf(stderr, "Initializinging...\n");
	reset();
	dumpPin();
	dumpReg();
	curPos = pin2Dec(PIN_IN_CK, 2);

	/* Start */
	while (1) {
		if ( PIN_OUT_UL) {
			/*** Unlocked ***/
			input = getchar();	
			
			switch (input) {
				case 'd':
					dumpPin();
					dumpReg();
					break;
				case 'n':
					bzero(PIN_IN_K, sizeof(pin) * 4);
					if (++curPos == 4) {
						PIN_OUT_UL = 0;	// Locked
						curPos = 0;
					}
					PIN_IN_CK[1] = (pin) (curPos / 2);
					PIN_IN_CK[0] = (pin) (curPos % 2);
					break;

				case '0':
				case '1':
					PIN_IN_K[bitpos] = (pin) (input-'0');
					KEY[curPos].d[bitpos--] = PIN_IN_K[bitpos];
			}

		} else {
			/*** Locked ***/
			input = getchar();	
			switch (input) {
				case 'd':
					dumpPin();
					dumpReg();
					break;
				case 'n':
					bzero(PIN_IN_K, sizeof(pin) * 4);
					if (++curPos == 4) {
						int i, match=1;

						/*** Key Matching ***/
						for (i=0; i<4 && match; i++) match = regCmp(KEY[i], BUF[i], 4);
						if (!match) {
							errCount++;
							ERR[0] = errCount %2;
							ERR[1] = errCount /2;
							ERR[2] = errCount /4;
						} else {
							PIN_OUT_UL = 1;	// Locked
						}
						curPos = 0;
						bzero(BUF, sizeof(struct registers) * 4);

					}
					PIN_IN_CK[1] = (pin) (curPos / 2);
					PIN_IN_CK[0] = (pin) (curPos % 2);
					break;

				case '0':
				case '1':
					PIN_IN_K[bitpos] = (pin) (input-'0');
					BUF[curPos].d[bitpos--] = PIN_IN_K[bitpos];
			}
		}

		if (bitpos == 255) bitpos = 3;
	}
}
