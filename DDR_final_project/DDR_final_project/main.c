#include <avr/io.h>
#include <avr/interrupt.h>
#include <bit.h>
#include <timer.h>
#include <stdio.h>
#include <io.c>
#include <string.h>

unsigned char GetKeypadKey() {

	PORTC = 0xEF; // Enable row 4 with 0, disable others with 1�s
	asm("nop"); // add a delay to allow PORTC to stabilize before checking
	if (GetBit(PINC,0)==0) { return('1'); }
	if (GetBit(PINC,1)==0) { return('4'); }
	if (GetBit(PINC,2)==0) { return('7'); }
	if (GetBit(PINC,3)==0) { return('*'); }

	// Check keys in col 2
	PORTC = 0xDF; // Enable row 3 with 0, disable others with 1�s
	asm("nop"); // add a delay to allow PORTC to stabilize before checking
	if (GetBit(PINC,0)==0) { return('2'); }
	if (GetBit(PINC,1)==0) { return('5'); }
	if (GetBit(PINC,2)==0) { return('8'); }
	if (GetBit(PINC,3)==0) { return('0'); }

	// Check keys in col 3
	PORTC = 0xBF; // Enable row 2 with 0, disable others with 1�s
	asm("nop"); // add a delay to allow PORTC to stabilize before checking
	if (GetBit(PINC,0)==0) { return('3'); }
	if (GetBit(PINC,1)==0) { return('6'); }
	if (GetBit(PINC,2)==0) { return('9'); }
	if (GetBit(PINC,3)==0) { return('#'); }

	// Check keys in col 4
	PORTC = 0X7F; // Enable row 1 with 0, disable others with 1�s
	asm("nop"); // add a delay to allow PORTC to stabilize before checking
	if (GetBit(PINC,0)==0) { return('A'); }
	if (GetBit(PINC,1)==0) { return('B'); }
	if (GetBit(PINC,2)==0) { return('C'); }
	if (GetBit(PINC,3)==0) { return('D'); }

	return('\0'); // default value

}


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

//--------Shared Variables----------------------------------------------------
unsigned char SM2_output = 0x00;
unsigned char SM3_output = 0x00;
unsigned char pressed = 0;
unsigned char key = 0x00;

//--------End Shared Variables------------------------------------------------



/*
SM's to make:

Audio
Input
Display
Score/Accuracy
Menu
	Song select
	Pause/exit/resume



*/

//--------User defined FSMs---------------------------------------------------
//Enumeration of states.
enum SM1_States { SM1_wait, SM1_press };

// Monitors button connected to PA0. 
// When button is pressed, shared variable "pause" is toggled.
int SMInputTick(int state) {
	
	//get input from controller
	key = GetKeypadKey();
	
	//State machine transitions
	switch (state) {
		case SM1_wait: pressed = 0;
				if (key != '\0') {	// Wait for button press
					state = SM1_press;
				}
			break;
		case SM1_press:	pressed = 1;
				if(key == '\0') {
					state = SM1_wait;
				}
			break;
		default: state = SM1_wait; // default: Initial state
			break;
	}

	//State machine actions
// 	switch(state) {
// 		case SM1_wait:	break;
// 		case SM1_press:	pause = (pause == 0) ? 1 : 0; // toggle pause
// 			break;
// 		case SM1_release:	break;
// 		default:		break;
// 	}

	return state;
}

//Enumeration of states.
enum SM2_States {SM2_wait, SM2_early, SM2_onTime, SM2_late };

// If paused: Do NOT toggle LED connected to PB0
// If unpaused: toggle LED connected to PB0
int SMLEDTick(int state) {
	int current = state;
	//State machine transitions
	switch (state) {
		case SM2_displayOff: state = SM2_wait;
			break;
		case SM2_wait: 
				if(pressed) {
					state = SM2_displayOn;
				}
			break;
		case SM2_displayOn: state = SM2_hold;
			break;
		case SM2_hold: 
				if(!pressed) {
					state = SM2_displayOff;
				}
			break;
		default: state = SM2_wait;
			break;
	}

	//State machine actions
	switch(current) {
		case SM2_displayOff:
				LCD_ClearScreen();
				LCD_DisplayString(1, "No input!");
			break;
		case SM2_wait: break;
		case SM2_displayOn:
				LCD_ClearScreen();
				LCD_WriteData(key);
			break;
		case SM2_hold: break;
		default: state = SM2_displayOff;	
			break;
	}

	return state;
}

// Implement scheduler code from PES.
int main()
{
// Set Data Direction Registers
// Buttons PORTA[0-7], set AVR PORTA to pull down logic
DDRA = 0xFF; PORTA = 0x00;
DDRB = 0xFF; PORTB = 0x00;
DDRC = 0xF0; PORTC = 0x0F;
DDRD = 0xFF; PORTD = 0x00;

// Period for the tasks
unsigned long int SMTick1_calc = 50;
unsigned long int SMTick2_calc = 50;
//unsigned long int SMTick3_calc = 1000;
//unsigned long int SMTick4_calc = 10;

//Calculating GCD
unsigned long int tmpGCD = 1;
tmpGCD = findGCD(SMTick1_calc, SMTick2_calc);
//tmpGCD = findGCD(tmpGCD, SMTick3_calc);
//tmpGCD = findGCD(tmpGCD, SMTick4_calc);

//Greatest common divisor for all tasks or smallest time unit for tasks.
unsigned long int GCD = tmpGCD;

//Recalculate GCD periods for scheduler
unsigned long int SMTick1_period = SMTick1_calc/GCD;
unsigned long int SMTick2_period = SMTick2_calc/GCD;
//unsigned long int SMTick3_period = SMTick3_calc/GCD;
//unsigned long int SMTick4_period = SMTick4_calc/GCD;

//Declare an array of tasks 
static task task1, task2;
task *tasks[] = { &task1, &task2 };
const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

// Task 1
task1.state = -1;//Task initial state.
task1.period = SMTick1_period;//Task Period.
task1.elapsedTime = SMTick1_period;//Task current elapsed time.
task1.TickFct = &SMKeypadTick;//Function pointer for the tick.

// Task 2
task2.state = -1;//Task initial state.
task2.period = SMTick2_period;//Task Period.
task2.elapsedTime = SMTick2_period;//Task current elapsed time.
task2.TickFct = &SMLEDTick;//Function pointer for the tick.

// Task 3
// task3.state = -1;//Task initial state.
// task3.period = SMTick3_period;//Task Period.
// task3.elapsedTime = SMTick3_period; // Task current elasped time.
// task3.TickFct = &SMTick3; // Function pointer for the tick.

// Task 4
// task4.state = -1;//Task initial state.
// task4.period = SMTick4_period;//Task Period.
// task4.elapsedTime = SMTick4_period; // Task current elasped time.
// task4.TickFct = &SMTick4; // Function pointer for the tick.

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
