
enum directions {zero, up, down, left, right, upDark, downDark, leftDark, rightDark};


struct music {
	double* tone;
	unsigned char* start;
	unsigned char* stop;
	unsigned char* direction;
	unsigned char current;
	unsigned int max;
	};

#define c4 261.63
#define d4 293.66
#define e4 329.63
#define f4 349.23
#define g4 392.00
#define a4 440.00
#define b4 493.88
#define c5 523.25

// music pianoMan(struct music song) {
// 	song.tone = {};
// 	song.start = {};
// 	song.stop = {};
// 	return song;
// }

const static double tones[] = {c4, d4, e4, f4, g4, a4, b4, c5, b4, a4, g4, f4, e4, d4, c4};
const static unsigned char starts[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29};
const static unsigned char stops[] = {2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30};
const static unsigned char directionS[] = {up, right, up, right, up, right, up, right, down, right, down, right, down, right, down};

struct music cScale(struct music song) {
	//double tones[] = {c4, d4, e4, f4, g4, a4, b4, c5, b4, a4, g4, f4, e4, d4, c4};
	//unsigned char starts[] = {0, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29};
	//unsigned char stops[] = {2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30};
	//unsigned char directionS[] = {up, right, up, right, up, right, up, right, down, right, down, right, down, right, down};
	song.tone = tones;
	song.start = starts;
	song.stop = stops;
	song.direction = directionS;
	song.current = 0;
	song.max = 30;
	return song;
}

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
