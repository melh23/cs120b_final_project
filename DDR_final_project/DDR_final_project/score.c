#include <avr/io.h>
#include <avr/eeprom.h>

unsigned char readMax() {
	return eeprom_read_byte(0x0F);
}

void writeMax(unsigned char score) {
	eeprom_write_byte(0x0F, score);
}
