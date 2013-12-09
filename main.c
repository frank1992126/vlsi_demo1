/*************************************************
 *  Author			: Frank Chang			 	 *
 *  Last Modified	: 2013/12/08			 	 *
 *  Description		: VLSI Combination Lock	Demo *
 *************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Color ASCII */
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

/* Type Definitions */
typedef unsigned char pin;
typedef struct registers {
  pin d[4];
} reg;
typedef unsigned char uint1;

/* Function Prototypes */
void dumpPin();
void dumpReg();
void reset();
int pin2Dec(pin*,int);
int regCmp(reg,reg,int);

/* Pin Declaration */
pin PIN_IN_K[4];			// Key input [3:0]
pin PIN_IN_CK[2];			// Current key position
pin PIN_OUT_UL;				// Unlocked
pin PIN_OUT_E5;				// 5 Errors
pin PIN_OUT_E3;				// 3 Errors

/* Register Declaration */
reg KEY[4];					// Current stored key
reg BUF[4];					// Input buffer
reg ERR;					// Error Counter

void dumpPin() {
	printf(KGRN "==== [Current Pin State] ====\n" KGRN);
	printf(KRED "PIN_IN_K:\t%s0b%1d%1d%1d%1d\t(%x)\n", KYEL,
												 PIN_IN_K[3],
												 PIN_IN_K[2],
												 PIN_IN_K[1],
												 PIN_IN_K[0],
												 pin2Dec(PIN_IN_K, 4));
	printf(KRED "PIN_IN_CK:\t%s0b%1d%1d\t(%d)\n", KYEL, PIN_IN_CK[1], PIN_IN_CK[0], pin2Dec(PIN_IN_CK, 2));
	printf(KRED "PIN_OUT_UL:\t%s0b%1d\t[%s]\n", KYEL, PIN_OUT_UL, PIN_OUT_UL?"UNLOCKED":"LOCKED");
	printf(KRED "PIN_OUT_E3:\t%s0b%1d\n", KYEL, PIN_OUT_E3);
	printf(KRED "PIN_OUT_E5:\t%s0b%1d\n", KYEL, PIN_OUT_E5);
}

void dumpReg() {
	printf(KGRN "==== [Current Reg State] ====\n" KGRN);
	printf(KRED "KEY:\t%s0x%x%x%x%x\n", KYEL,
								 pin2Dec(KEY[0].d, 4),
								 pin2Dec(KEY[1].d, 4),
								 pin2Dec(KEY[2].d, 4),
								 pin2Dec(KEY[3].d, 4));
	printf(KRED "BUF:\t%s0x%x%x%x%x\n", KYEL,
								 pin2Dec(BUF[0].d, 4),
								 pin2Dec(BUF[1].d, 4),
								 pin2Dec(BUF[2].d, 4),
								 pin2Dec(BUF[3].d, 4));
	printf(KRED "ERR:\t%s0x%x\t(%d)\n" KNRM, KYEL, pin2Dec(ERR.d,3), pin2Dec(ERR.d,3));
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
	bzero(&ERR, sizeof(struct registers) * 1);
	PIN_OUT_UL = 1;
	PIN_OUT_E5 = 0;
	PIN_OUT_E3 = 0;
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
	while (1 && !PIN_OUT_E5) {
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
							ERR.d[2]=(errCount >> 2) & 1;;
							ERR.d[1]=(errCount >> 1) & 1;
							ERR.d[0]=errCount & 1;
						} else {
							errCount = 0;
							bzero(&ERR, sizeof(struct registers));
							PIN_OUT_UL = 1;	// Locked
						}

						if (errCount == 3) {
							PIN_OUT_E3 = 1;
						} else if (errCount == 5) {
							PIN_OUT_E3 = 0;
							PIN_OUT_E5 = 1;
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

