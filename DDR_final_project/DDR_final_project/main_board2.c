/*
 * board2.c
 *
 * Created: 6/5/2019 9:21:16 PM
 * Author : melhe
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define zero 0
#define a3 220.00
#define a3s 233.08
#define b3 246.94
#define c4 261.63
#define c4s 277.18
#define d4 293.66
#define d4s 311.13
#define e4 329.63
#define f4 349.23
#define f4s 369.99
#define g4 392.00
#define g4s 415.31
#define a4 440.00
#define a4s 466.16
#define b4 493.88
#define c5 523.25
#define c5s 554.37
#define d5 587.33

double notes[] = {zero, a3, a3s, b3, c4, c4s, d4, d4s, e4, f4, f4s, g4, g4s, a4, a4s, b4, c5, c5s, d5};


void set_PWM(double frequency) {
	static double current_frequency;
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; }
		else { TCCR3B |= 0x03; }
		
		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		else if (frequency > 31250) { OCR3A = 0x0000; }
		
		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }

		TCNT3 = 0;
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}



int main(void)
{
    DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	unsigned char varA = 0x00;
	
	PWM_on();
	set_PWM(0);
	
    while (1) 
    {
		varA = (PINA) & 0x1F;
		PORTC = (PORTC & 0x00) | varA;
		set_PWM(notes[varA]);
		//set_PWM(261.63);
		
    }
}

