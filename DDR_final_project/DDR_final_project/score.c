#include <avr/io.h>
#include <avr/eeprom.h>


struct string {
	unsigned char str[50];
	unsigned char size;
	
	};

unsigned char getSize(const unsigned char* s) {
	unsigned char i;
	for(i = 0; s[i] != '\0'; i += 1)
	return i;
}

void setString(struct string myStr, const unsigned char* s) {
	unsigned char size = getSize(s);
	myStr.size = size;
	for(unsigned char i = 0; i < size; i +=1){
		myStr.str[i] = s[i];
	}
}

unsigned char appendStr(struct string dest, struct string src) {
	strcat(dest.str, src.str);
	dest.size = dest.size + src.size + 1;
	dest.str[dest.size] = '\0';
}

unsigned char readMax() {
	
	return eeprom_read_byte(0x0F);
	
// 	unsigned char sreg = SREG;
// 	cli();
// 	
// 	/* Wait for completion of previous write */
// 	while(EECR & (1<<EEPE));
// 	
// 	/* Set up address register */
// 	EEAR = 20;
// 	
// 	/* Start eeprom read by writing EERE */
// 	EECR |= (1<<EERE);
// 	
// 	PORTB = EEDR;
// 	
// 	SREG = sreg;
// 	sei();
// 	
// 	/* Return data from data register */
// 	return EEDR;
}

void writeMax(unsigned char score) {
	eeprom_write_byte(0x0F, score);
// 	unsigned char sreg = SREG;
// 	cli();
//  
// 	 /* Wait for completion of previous write */
// 	 while(EECR & (1<<EEPE));
//  
// 	 /* Set up address and data registers */
// 	 EEAR = 20;
// 	 EEDR = score;
//  
// 	 /* Write logical one to EEMPE */
// 	 EECR |= (1<<EEMPE);
//  
// 	 /* Start eeprom write by setting EEWE */
// 	 EECR |= (1<<EEPE);
// 	 
// 	 SREG = sreg;
//	 sei();
}
