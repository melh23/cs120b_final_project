#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "io.h"

#define SET_BIT(p,i) ((p) |= (1 << (i)))
#define CLR_BIT(p,i) ((p) &= ~(1 << (i)))
#define GET_BIT(p,i) ((p) & (1 << (i)))

/*-------------------------------------------------------------------------*/

#define DATA_BUS PORTD		// port connected to pins 7-14 of LCD display
#define CONTROL_BUS PORTC	// port connected to pins 4-6 of LCD display
#define DATA_CMD DDRD		// ddrx connected to pins 7-14 of LCD display
#define CONTROL_CMD DDRC	// ddrx connected to pins 4-6 of LCD display
#define RS 1			// pin number of uC connected to pin 4 of LCD disp.
//#define RW 1		//pin number of uC connected to pin 5 of LCD disp.
#define E 0			// pin number of uC connected to pin 6 of LCD disp.

/*-------------------------------------------------------------------------*/

//may want to save these onto EEPROM? idk
const static unsigned char upA[8] = {0x04, 0x0A, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00};
const static unsigned char downA[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x0A, 0x04};
const static unsigned char leftA[8] = {0x00, 0x04, 0x08, 0x10, 0x08, 0x04, 0x00, 0x00};
const static unsigned char rightA[8] = {0x00, 0x04, 0x02, 0x01, 0x02, 0x04, 0x00, 0x00};
const static unsigned char upDarkA[8] = {0x1B, 0x15, 0x0E, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F};
const static unsigned char downDarkA[8] = {0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x0E, 0x15, 0x1B};
const static unsigned char leftDarkA[8] = {0x1B, 0x17, 0x17, 0x0F, 0x0F, 0x17, 0x17, 0x1B};
const static unsigned char rightDarkA[8] = {0x1B, 0x1D, 0x1D, 0x1E, 0x1E, 0x1D, 0x1D, 0x1B};

/*-------------------------------------------------------------------------*/

void LCD_ClearScreen(void) {
   LCD_WriteCommand(0x01);
}

void LCD_init(void) {

	//CONTROL_CMD = 0xFF;		//allow write data to memory?
	//DATA_CMD = 0xFF;
	
    //wait for 100 ms.
	delay_ms(100);
	LCD_WriteCommand(0x38);
	LCD_WriteCommand(0x06);
	LCD_WriteCommand(0x0f);
	LCD_WriteCommand(0x01);
	delay_ms(10);						 
}

void LCD_WriteCommand (unsigned char Command) {
   CLR_BIT(CONTROL_BUS,RS);
   DATA_BUS = Command;
   SET_BIT(CONTROL_BUS,E);
   asm("nop");
   CLR_BIT(CONTROL_BUS,E);
   delay_ms(2); // ClearScreen requires 1.52ms to execute
}

void LCD_WriteData(unsigned char Data) {
   SET_BIT(CONTROL_BUS,RS);
   DATA_BUS = Data;
   SET_BIT(CONTROL_BUS,E);
   asm("nop");
   CLR_BIT(CONTROL_BUS,E);
   delay_ms(1);
}

void LCD_DisplayStringNoClear(unsigned char column, const unsigned char* string) {
	unsigned char c = column;
	while(*string) {
		LCD_Cursor(c++);
		LCD_WriteData(*string++);
	}
}

void LCD_DisplayString(unsigned char column, const unsigned char* string) {
   LCD_ClearScreen();
   unsigned char c = column;
   while(*string) {
      LCD_Cursor(c++);
      LCD_WriteData(*string++);
   }
}

void LCD_Cursor(unsigned char column) {
   if ( column < 17 ) { // 16x1 LCD: column < 9
						// 16x2 LCD: column < 17
      LCD_WriteCommand(0x80 + column - 1);
   } else {
      LCD_WriteCommand(0xB8 + column - 9);	// 16x1 LCD: column - 1
											// 16x2 LCD: column - 9
   }
}

void delay_ms(int miliSec) //for 8 Mhz crystal
{
    int i,j;
    for(i=0;i<miliSec;i++)
    for(j=0;j<775;j++)
  {
   asm("nop");
  }
}

//custom char fxns

void LCD_LoadChar(unsigned char address, unsigned char* symbol) {
	LCD_WriteCommand(0x40 + (address*8));	//for ccgram
	for(unsigned int i = 0; i < 8; i++) {
		LCD_WriteData(symbol[i]);
	}
	LCD_WriteCommand(0x80);
}

void LCD_LoadCustomChars() {
	LCD_LoadChar(1, upA);
	LCD_LoadChar(1, upA);
	LCD_LoadChar(2, downA);
	LCD_LoadChar(3, leftA);
	LCD_LoadChar(4, rightA);
	LCD_LoadChar(5, upDarkA);
	LCD_LoadChar(6, downDarkA);
	LCD_LoadChar(7, leftDarkA);
	LCD_LoadChar(8, rightDarkA);
}