#include <avr/io.h>
#include <avr/interrupt.h>
#include <bit.h>
#include <timer.h>
#include <stdio.h>
#include <io.c>
#include <string.h>
#include <keypad.h>
#include <score.c>

//--------Find GCD function --------------------------------------------------
unsigned long int findGCD(unsigned long int a, unsigned long int b)
{
	unsigned long int c;
	while(1){
		c = a%b;
		if(c==0){return b;}
		a = b;
b = c;
	}
	return 0;
}
//--------End find GCD function ----------------------------------------------

//--------Task scheduler data structure---------------------------------------
// Struct for Tasks represent a running process in our simple real-time operating system.
typedef struct _task {
	/*Tasks should have members that include: state, period,
		a measurement of elapsed time, and a function pointer.*/
	signed char state; //Task's current state
	unsigned long int period; //Task period
	unsigned long int elapsedTime; //Time elapsed since last task tick
	int (*TickFct)(int); //Task tick function
} task;

//--------End Task scheduler data structure-----------------------------------

typedef enum {false, true} bool;

//--------Shared Variables----------------------------------------------------
unsigned char button;
unsigned char player1 = 0;
unsigned char player2 = 0;
unsigned char pressed = 0;
bool menu = true;
bool pause = false;

//--------End Shared Variables------------------------------------------------


//--------User defined FSMs---------------------------------------------------
//Enumeration of states.
enum SNES_SM { SNES_wait, SNES_press, SNES_hold };

// Monitors button connected to PA0. 
// When button is pressed, shared variable "pause" is toggled.
int SNESInputTick(int state) {
	
	//get input from controller
// 	button = GetSNESIn();
// 	player1 = mapSNESIn(button, 1);
// 	player2 = mapSNESIn(button, 2);
// 	menu = mapSNESIn(button, 0);
	button = GetKeypadKey();
	
	//State machine transitions
	switch (state) {
		case SNES_wait: pressed = 0;
				// if (!button.empty()) {	// If there are items in button, an input has been entered
				if(button != '\0') {
					state = SNES_press;
				}
			break;
		case SNES_press: state = SNES_hold;	pressed = 1;
			break;
		case SNES_hold:
				//if(button.empty()) {	// If there are no items in button, no input is currently available
				if(button == '\0') {
					state = SNES_wait;
				}
			break;
		default: state = SNES_wait; // default: Initial state
			break;
	}

	//State machine actions
	switch(state) {
		case SNES_wait: PORTB = 0x00;	
			break;
		case SNES_press:
				PORTB = 0xFF;
				switch(button) {
					case 'A': if(player1 < 240) player1 += 10;
						break;
					case 'B': if(player1 > 10) player1 -= 10;
						break;
					case 'C': if(player2 < 240) player2 += 10;
						break;
					case 'D': if(player2 > 10) player2 -= 10;
						break;
					case '5': writeMax(0x00);
						break;
					default:
						break;
				}
			break;
		case SNES_hold:	
			break;
		default:		
			break;
	}

	return state;
}

//Audio SM
enum Audio_SM {Audio_wait, Audio_play };

int AudioTick(int state) {
	
	//State machine transitions
	switch (state) {
		case Audio_wait: 
			break;
		case Audio_play: 
				
			break;
		default: state = Audio_wait;
			break;
	}

	//State machine actions
	switch(state) {
		case Audio_wait:
			break;
		case Audio_play: 
			break;
		default: state = Audio_wait;	
			break;
	}

	return state;
}

//LED SM
enum LED_SM { LED_wait };
	
int LEDTick(int state) {
	
	//State machine transitions
	switch (state) {
		case LED_wait: state = LED_wait;
			break;
// 		case :
// 			break;
		default: state = LED_wait;
			break;
	}

	//State machine actions
// 	switch(state) {
// 		case :
// 			break;
// 		case :
// 			break;
// 		default: state = ;
// 			break;
// 	}
	return state;
}

//LCD SM
enum LCD_SM { LCD_start, LCD_main, LCD_max, LCD_current, LCD_display, LCD_pause };

int LCDTick(int state) {
	
	//State machine transitions
	switch(state) {
		case LCD_start: state = LCD_main; menu = true;
		case LCD_main: 
				switch(button) {
					case '#': state = LCD_current;
						break;
					case '*': state = LCD_max;
						break;
					default: state = LCD_main;
						break;
				}
			break;
		case LCD_max: state = LCD_display;
			break;
		case LCD_current: state = LCD_display; menu = false;
			break;
		case LCD_display:
				if(button == '0') {
					state = LCD_start;
				}
// 				if(button == '0' && menu) {
// 					state = LCD_start;
// 				} else if(button == '0' && !menu) {
// 					state = pause ? LCD_current : LCD_pause;
// 					pause = pause ? false : true;
// 				}
			break;
		case LCD_pause: state = LCD_display;
			break;
		default: state = LCD_start;
			break;
	}
	
	//State machine actions
	switch (state) {
		case LCD_start:
				LCD_ClearScreen();
				LCD_DisplayString(1, "Main menu");
	 		break;
		case LCD_main: 
			break;
	 	case LCD_max:{
				 unsigned char maxScore = readMax();
				 if(player1 > maxScore) {
					maxScore = player1;
					writeMax(player1);
				 }
				 if(player2 > maxScore) {
					maxScore = player2;
					writeMax(player2);
				 }
				 
				 PORTB = (PORTB & 0x00) | maxScore;
				 unsigned char buffer[10];
				 unsigned char printout[34] = "Max Score: ";
				 itoa(maxScore, buffer, 10);
				 strcat(printout, buffer);
				 printout[14] = '\0';
				 printout[15] = '\0';
				 LCD_ClearScreen();
				 LCD_DisplayString(1, printout);
			 }
	 		break;
	 	case LCD_current:{
				//unsigned char* currentScores = updateLCDString(player1, player2);
				//LCD_DisplayString(1, currentScores);
				
				unsigned char buffer1[10];
				unsigned char buffer2[10];
				
				itoa(player1, buffer1, 10);
				itoa(player2, buffer2, 10);
				
				unsigned char printout[34] = "Player 1: ";
				strcat(printout, buffer1);
				strcat(printout, "     Player 2: ");
				strcat(printout, buffer2);
				
				LCD_ClearScreen();
				LCD_DisplayString(1, printout);
				
				PORTB = (PORTB & 0x00) | player1;
			}
	 		break;
		case LCD_pause: 
				LCD_ClearScreen();
				LCD_DisplayString(1, "Puased!");
			break;	
	 	default:
	 		break;
	}
	
	return state;
}


// Implement scheduler code from PES.
int main()
{
	//port A and B are for LCD
	//port C is for LED  //temporarily for keypad
	//port D is for audio
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xF0; PORTC = 0x0F;
	DDRD = 0xFF; PORTD = 0x00;

	// Period for the tasks
	unsigned long int SNESTick_calc = 50;
	unsigned long int AudioTick_calc = 50;
	unsigned long int LEDTick_calc = 50;
	unsigned long int LCDTick_calc = 50;

	//Calculating GCD
	unsigned long int tmpGCD = 1;
	tmpGCD = findGCD(SNESTick_calc, AudioTick_calc);
	tmpGCD = findGCD(tmpGCD, LEDTick_calc);
	tmpGCD = findGCD(tmpGCD, LCDTick_calc);

	//Greatest common divisor for all tasks or smallest time unit for tasks.
	unsigned long int GCD = tmpGCD;

	//Recalculate GCD periods for scheduler
	unsigned long int SNESTick_period = SNESTick_calc/GCD;
	unsigned long int AudioTick_period = AudioTick_calc/GCD;
	unsigned long int LEDTick_period = LEDTick_calc/GCD;
	unsigned long int LCDTick_period = LCDTick_calc/GCD;

	//Declare an array of tasks 
	static task task1, task2, task3, task4;
	task *tasks[] = { &task1, &task2, &task3, &task4 };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	// Task 1
	task1.state = -1;//Task initial state.
	task1.period = SNESTick_period;//Task Period.
	task1.elapsedTime = SNESTick_period;//Task current elapsed time.
	task1.TickFct = &SNESInputTick;//Function pointer for the tick.

	// Task 2
	task2.state = -1;//Task initial state.
	task2.period = AudioTick_period;//Task Period.
	task2.elapsedTime = AudioTick_period;//Task current elapsed time.
	task2.TickFct = &AudioTick;//Function pointer for the tick.

	// Task 3
	task3.state = -1;//Task initial state.
	task3.period = LEDTick_period;//Task Period.
	task3.elapsedTime = LEDTick_period; // Task current elasped time.
	task3.TickFct = &LEDTick; // Function pointer for the tick.

	// Task 4
	task4.state = -1;//Task initial state.
	task4.period = LCDTick_period;//Task Period.
	task4.elapsedTime = LCDTick_period; // Task current elasped time.
	task4.TickFct = &LCDTick; // Function pointer for the tick.

	// Set the timer and turn it on
	TimerSet(GCD);
	TimerOn();
	LCD_init();

	unsigned short i; // Scheduler for-loop iterator
	while(1) {
	
		// Scheduler code
		for ( i = 0; i < numTasks; i++ ) {
			// Task is ready to tick
			if ( tasks[i]->elapsedTime == tasks[i]->period ) {
				// Setting next state for task
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				// Reset the elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}

	// Error: Program should not exit!
	return 0;
}
