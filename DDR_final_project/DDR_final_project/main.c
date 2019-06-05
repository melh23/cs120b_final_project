#include <avr/io.h>
#include <avr/interrupt.h>
#include <bit.h>
#include <timer.h>
#include <stdio.h>
#include <io.c>
#include <string.h>
#include <score.c>
#include <music.c>
#include <snes.c>


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

/*typedef enum {false, true} bool;*/

//--------Shared Variables----------------------------------------------------
unsigned char ins[12];
unsigned int player1 = 0;
unsigned int player2 = 0;
bool scoreChange = false;
unsigned char pressed = 0;
bool menu = true;
bool pause = false;
struct music scale; 
unsigned short beat = 0;
bool song_over = false;
bool prevIns[12];
unsigned char* wholeSong;
unsigned char* displayString;

//--------End Shared Variables------------------------------------------------


//--------User defined FSMs---------------------------------------------------

//SNES SM
enum SNES_SM { SNES_wait, SNES_press, SNES_hold };

int SNESInputTick(int state) {
	
	//get input from controller
 	unsigned short button = GetSNESIn();
	PORTB = (PORTB & 0x00) | button;	//for debugging
	mapPlayerInput(button, ins);
	
	//State machine transitions
	switch (state) {
		case SNES_wait: pressed = 0;
				if(button != 0) {
					state = SNES_press;
				}
			break;
		case SNES_press: state = SNES_hold;	pressed = 1;
			break;
		case SNES_hold:
				if(!button) {
					state = SNES_wait;
				}
			break;
		default: state = SNES_wait; // default: Initial state
			break;
	}

	//State machine actions
	switch(state) {
		case SNES_wait:
			break;
		case SNES_press:	//cheat codes ;)
				if(menu && inputContains(ins, L) && inputContains(ins, R)) {	//L and R @ same time resets max score
					writeMax(0x00);
				}
				if(!menu && !pause) {		//change P1 score while game is running
					if(player1 < 240 && inputContains(ins, R)) {
						player1 += 10;
						scoreChange = true;
					}
					if(player1 > 9 && inputContains(ins, L)) {
						player1 -= 10;
						scoreChange = true;
					}
				}
			break;
		case SNES_hold:	
			break;
		default:		
			break;
	}

	return state;
}

void LCD_WriteValue(unsigned int value) {
	unsigned char buffer[10];
	itoa(value, buffer, 10);
	LCD_DisplayString(1, buffer);
}

//Audio SM
enum Audio_SM {Audio_wait, Audio_start, Audio_play , Audio_off , Audio_pause};

int AudioTick(int state) {	
	
	//State machine transitions
	switch (state) {
		case Audio_wait: 
				beat = 0;
				if(menu == false) {
					if(beat < scale.start[scale.current]) {
						state = Audio_off;
					} else {
						state = Audio_start;
					}
				}
			break;
		case Audio_start: state = Audio_play;
			break;
		case Audio_play:
				if(beat >= scale.stop[scale.current] && pause == false) {
					state = Audio_off;
					scale.current += 1;
				} else if(pause == true) {
					state = Audio_pause;
				}
				beat += 1;
			break;
		case Audio_off:
				if(beat >= scale.start[scale.current] && pause == false) {
					state = Audio_start;
				} else if(pause == true) {
					state = Audio_pause;
				}
				beat += 1;
			break;
		case Audio_pause:
				if(pause == false && menu == false) {
					if(scale.start[scale.current] <= beat) {
						state = Audio_start;
					} else if(scale.stop[scale.current] <= beat) {
						state = Audio_off;
					} else {
						state = Audio_play;
					}
				} else if(menu == true) {
					state = Audio_wait;
					scale.current = 0;
				}
			break;
		default: state = Audio_wait;
			break;
	}
	
	//State machine actions
	switch(state) {
		case Audio_wait: set_PWM(0);
			break;
		case Audio_start: set_PWM(scale.tone[scale.current]);
			break;
		case Audio_play: 
			break;
		case Audio_off: set_PWM(0);
			break;
		case Audio_pause: set_PWM(0);
			break;
		default: set_PWM(0);	
			break;
	}
	
	//when at end of song, return to main menu
	if(beat > scale.max && menu == false && pause == false) {
		state = Audio_wait;
		scale.current = 0;
		song_over = true;
		beat = 0;
	}

	return state;
}

//Timer SM
//basically in game display tick
enum TIMER_SM { Timer_wait, Timer_update };
	
int TimerTick(int state) {
	
	//State machine transitions
	switch (state) {
		case Timer_wait:	//as long as the game is not running, wait
				if(!menu && !pause) {
					state = Timer_update;
				}
			break;
 		case Timer_update:
				if(menu || pause) {
					state = Timer_wait;
				}
 			break;
		default: state = Timer_wait;
			break;
	}

	//State machine actions
	switch(state) {
		case Timer_wait:
			break;
		case Timer_update:
				displayString = updateSongString(wholeSong, beat, displayString, scale.max);
				LCD_DisplayStringNoClear(1, displayString);
				if(scoreChange) {
					updatePoints();
					scoreChange = false;
				}
			break;
		default:
			break;
	}
	return state;
}

void updatePoints() {
	unsigned char buffer[10];
	
	itoa(player1, buffer, 10);
	for(unsigned char i = 0; i < strlen(buffer); i++) {
		LCD_Cursor(20 + i);	//P1: at very beginning of 2nd row
		LCD_WriteData(buffer[i]);
	}
	
	itoa(player2, buffer, 10);
	for(unsigned char i = 0; i < strlen(buffer); i++) {
		LCD_Cursor(27 + i);	//P2: at middle of 2nd row
		LCD_WriteData(buffer[i]);
	}
}

void updateMax() {
	unsigned char maxScore = readMax();
	if(player1 > maxScore) {
		maxScore = player1;
		writeMax(player1);
	}
	if(player2 > maxScore) {
		maxScore = player2;
		writeMax(player2);
	}
}

//LCD SM
enum LCD_SM { LCD_start, LCD_main, LCD_max, LCD_current, LCD_menuDisplay, LCD_gameDisplay, LCD_pause };

int LCDTick(int state) {
	
	//State machine transitions
	switch(state) {
		case LCD_start: state = LCD_main; menu = true; pause = false; song_over = false; player1 = 0; player2 = 0;
		case LCD_main: 
				menu = true; pause = false;
				if(inputContains(ins, Select)) {
					state = LCD_current;
				} else if(inputContains(ins, L)) {
					state = LCD_max;
				} 
			break;
		case LCD_max: state = LCD_menuDisplay; menu = true; pause = false;
			break;
		case LCD_current: state = LCD_gameDisplay; menu = false; pause = false;
				if(song_over == true) {
					state = LCD_start;
					menu = true;
					pause = false;
				}
			break;
		case LCD_menuDisplay:
				menu = true;
				if(inputContains(ins, Start)) {
					state = LCD_start;
				}
			break;
		case LCD_gameDisplay:
				menu = false;
				if(inputContains(ins, Select) && !pause) {	//pause game
					state = LCD_pause;
				} else if(inputContains(ins, A) && pause) {	//resume game
					state = LCD_current;
				} else if(pause && inputContains(ins, Start)) {		//return to main menu from paused game
					state = LCD_start;
 				}
				
				//return to main menu at end of song
				if(song_over == true) {
					state = LCD_start;
					menu = true;
					pause = false;
				}
			break;
		case LCD_pause: state = LCD_gameDisplay;
				menu = false;
				pause = true;
			break;
		default: state = LCD_start;
			break;
	}
	
	//State machine actions
	switch (state) {
		case LCD_start: {
				//save max score
				updateMax();
				LCD_ClearScreen();
				LCD_DisplayString(1, "Main menu");
				menu = true;
				pause = false;
			}
	 		break;
		case LCD_main: 
			break;
	 	case LCD_max:{
				 unsigned char maxScore = readMax();
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
	 	case LCD_current:
				pause = false;
				menu = false;
				
				//set up point display for game
				unsigned char play[32] = "                P1:    P2:     ";
				LCD_DisplayString(1, play);
				updatePoints();
	 		break;
		case LCD_pause: 
				updateMax();
				pause = true;
				LCD_ClearScreen();
				LCD_DisplayString(1, "Paused!");
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
	DDRA = 0xFE; PORTA = 0x01;	//need input for snes controller
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;	//was F0 and 0F for keypad
	DDRD = 0xFF; PORTD = 0x00;
	
	//for DDRX:
	//0 for output
	//1 for input

	// Period for the tasks
	unsigned long int SNESTick_calc = 50;
	unsigned long int AudioTick_calc = 100;
	unsigned long int TimerTick_calc = 50;
	unsigned long int LCDTick_calc = 50;

	//Calculating GCD
	unsigned long int tmpGCD = 1;
	tmpGCD = findGCD(SNESTick_calc, AudioTick_calc);
	tmpGCD = findGCD(tmpGCD, TimerTick_calc);
	tmpGCD = findGCD(tmpGCD, LCDTick_calc);

	//Greatest common divisor for all tasks or smallest time unit for tasks.
	unsigned long int GCD = tmpGCD;

	//Recalculate GCD periods for scheduler
	unsigned long int SNESTick_period = SNESTick_calc/GCD;
	unsigned long int AudioTick_period = AudioTick_calc/GCD;
	unsigned long int TimerTick_period = TimerTick_calc/GCD;
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
	task3.period = TimerTick_period;//Task Period.
	task3.elapsedTime = TimerTick_period; // Task current elasped time.
	task3.TickFct = &TimerTick; // Function pointer for the tick.

	// Task 4
	task4.state = -1;//Task initial state.
	task4.period = LCDTick_period;//Task Period.
	task4.elapsedTime = LCDTick_period; // Task current elasped time.
	task4.TickFct = &LCDTick; // Function pointer for the tick.

	// Set the timer and turn it on
	TimerSet(GCD);
	TimerOn();
	LCD_init();
	LCD_LoadCustomChars();
	scale = cScale(scale);
	PWM_on();
	unsigned char whole[scale.max + 3];
	unsigned char display[33];
	wholeSong = whole;
	displayString = display;
	wholeSong = generateSongString(scale, wholeSong);

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
