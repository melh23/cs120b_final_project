#include <stdio.h>


enum directions {zero, up, down, left, right, upDark, downDark, leftDark, rightDark};


struct music {
	double* tone;
	unsigned char* start;
	unsigned char* stop;
	unsigned char* direction;
	unsigned char current;
	unsigned int max;
	unsigned char tones;
	};

#define noFreq 0.00
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

double notes[] = {noFreq, a3, a3s, b3, c4, c4s, d4, d4s, e4, f4, f4s, g4, g4s, a4, a4s, b4, c5, c5s, d5};

const static double scaleTones[] = {c4, d4, e4, f4, g4, a4, b4, c5, b4, a4, g4, f4, e4, d4, c4};
const static unsigned char scaleStarts[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29};
const static unsigned char scaleStops[] = {2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30};
const static unsigned char scaleDirections[] = {up, right, up, right, up, right, up, right, down, right, down, right, down, right, down};
const static unsigned int scaleMax = 30;
const static unsigned char scaleTone = 15;

const static double mTones[] = {d4, d4, d5, a4, g4s, g4, f4, f4, d4, f4, g4};
const static unsigned char mStarts[] = {1, 2, 3, 5, 7, 9, 11, 12, 13, 14, 15};
const static unsigned char mStops[] = {2, 3, 5, 7, 8, 10, 12, 13, 14, 15, 16};
const static unsigned char mDirections[] = {down, down, up, right, right, right, down, down, right, up};
	
const static double starTones[] = {c4, c4, g4, g4, a4, a4, g4, f4, f4, e4, e4, d4, d4, c4};
const static unsigned char starStarts[] = {17, 19, 21, 23, 25, 27, 29, 33, 35, 37, 39, 41, 43, 45};
const static unsigned char starStops[] = {18, 20, 22, 24, 26, 28, 31, 34, 36, 38, 40, 42, 44, 47};
const static unsigned char starDirections[] = {left, left, right, right, up, up, down, up, up, right, right, left, left, down};
const static unsigned int starMax = 47;
const static unsigned char starTone = 15;

const static double birthTones[] = {d4, e4, d4, g4, f4, d4, e4, d4, a4, g4, d4, d5, b4, g4, f4, e4, c5, b4, g4, a4, g4};
const static unsigned char birthStarts[] = {1, 3, 5, 7, 9, 13, 15, 17, 19, 21, 25, 27, 29, 31, 33, 35, 37, 39, 43, 45, 47, 49, 51, 53};
const static unsigned char birthStops[] =  {2, 4, 6, 8, 12, 14, 16, 18, 20, 22, 24, 28, 30, 32, 34, 36, 38, 40, 44, 46, 48, 50, 52, 56};
const static unsigned char birthDirections[] = {left, right, left, up, down, left, right, left, up, down, left, right, left, up, down, left, right, left, up, down};
const static unsigned int birthMax = 56;

// const static unsigned char starStarts[] = {1, 3, 5, 7, 9, 11, 13, 17, 19, 21, 23, 25, 27, 29};
// const static unsigned char starStops[] = {2, 4, 6, 8, 10, 12, 15, 18, 20, 22, 24, 26, 28, 31};


struct music birthday(struct music song) {
	song.tone = birthTones;
	song.start = birthStarts;
	song.stop = birthStops;
	song.direction = birthDirections;
	song.current = 0;
	song.tones = 21;
	song.max = 56;
	return song;
}

struct music littleStar(struct music song) {
	song.tone = starTones;
	song.start = starStarts;
	song.stop = starStops;
	song.direction = starDirections;
	song.current = 0;
	song.tones = starTone;
	song.max = starMax;
	return song;
}

struct music megalovania(struct music song) {
	song.tone = mTones;
	song.start = mStarts;
	song.stop = mStops;
	song.direction = mDirections;
	song.current = 0;
	song.max = 16;
	song.tones = 12;
	return song;
}

struct music cScale(struct music song) {
	song.tone = scaleTones;
	song.start = scaleStarts;
	song.stop = scaleStops;
	song.direction = scaleDirections;
	song.current = 0;
	song.max = scaleMax;
	song.tones = scaleTone;
	return song;
}

unsigned char* updateSongString(unsigned char* wholeSong, unsigned char beat, unsigned char* currentStr, unsigned char maxLength) {
	//input characters
	if(beat < maxLength - 16) {
		strncpy(currentStr, (wholeSong + beat), 16);
	} else {
		unsigned char emptystr[20] = {"                 "};
		strcpy(currentStr, emptystr);
		strncpy(currentStr, (wholeSong + beat), (maxLength - beat));
	}
	
 	currentStr[16] = '\0';

	return currentStr;
}

unsigned char* generateSongString(struct music song, unsigned char* str) {
	unsigned char current = 0;
	unsigned char* padding = "                ";
	unsigned char s[song.max + 1];
	
	for(unsigned char i = 0; i < song.max; i++) {
		if(song.start[current] == i) {
			str[i] = song.direction[current];
			current++;
		} else {
			str[i] = ' ';
		}
	}
	
// 	strcpy(str, padding);
// 	strcat(str, s);
// 	strcat(str, padding);
	
	return str;
}

// void set_PWM(double frequency) {
// 	static double current_frequency;
// 	if (frequency != current_frequency) {
// 		if (!frequency) { TCCR3B &= 0x08; }
// 		else { TCCR3B |= 0x03; }
// 		
// 		if (frequency < 0.954) { OCR3A = 0xFFFF; }
// 		else if (frequency > 31250) { OCR3A = 0x0000; }
// 		
// 		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }
// 
// 		TCNT3 = 0;
// 		current_frequency = frequency;
// 	}
// }
// 
// void PWM_on() {
// 	TCCR3A = (1 << COM3A0);
// 	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
// 	set_PWM(0);
// }
// 
// void PWM_off() {
// 	TCCR3A = 0x00;
// 	TCCR3B = 0x00;
// }
