#define F_CPU 8000000UL // Clock Definition 8MHz

#include <avr/io.h>
#include <util/delay.h>
#include "bit.h"

typedef enum {false, true} bool;

#define SET_BIT(p,i) ((p) |= (1 << (i)))
#define CLR_BIT(p,i) ((p) &= ~(1 << (i)))
#define GET_BIT(p,i) ((p) & (1 << (i)))

#define CLOCK  2 // Output
#define LATCH  1 // Output
#define DATA   0 // Input

enum inputs {none, B, Y, Select, Start, Up, Down, Left, Right, A, X, L, R};

unsigned short GetSNESIn()
{
	unsigned short data = 0x0000;
	
	//request data from controller
	PORTA = SetBit(PINA, LATCH, 1);
	_delay_us(12);
	PORTA = SetBit(PINA, LATCH, 0);
	_delay_us(6);

	//read data
	for (unsigned char i = 0; i < 12; i++) {
		if(!GetBit(PINA, DATA)) {
			SET_BIT(data, i);
		} else {
			CLR_BIT(data, i);
		}
		
		//tick clock and wait for next bit 
		PORTA = SetBit(PINA, CLOCK, 1);
		_delay_us(6);
		PORTA = SetBit(PINA, CLOCK, 0);
		_delay_us(6);
	}

	return data;
}

void mapPlayerInput(unsigned short rawInput, unsigned char* ins) {
	unsigned char count = 0;
	for(unsigned char i = 0; i < 12; i++) {
		if(GET_BIT(rawInput, i)) {
			ins[count] = i + 1;
			count++;
		}
	}
	ins[count] = 0;
}

bool inputContains(unsigned char* ins, unsigned char input) {
	for(unsigned char i = 0; i < 12; i++) {
		if(ins[i] == input) {
			return true;
		} else if(ins[i] == 0) {
			return false;
		}
	}
	return false;
}


