/*
 EEPROMText.h - Library for reading or writing texts from or to an EEPROM
 Created by Adri Timmermans, September 24, 2020
 Private release
 
 prerequisites: I2C bus is activated befor first call to a read/write action
 
*/
#ifndef EEPROMText_h
#define EEPROMText_h
#include "Arduino.h"


class EEPROMText
{

	public:

		EEPROMText (int I2CChannel, byte address, byte csPin, int returnChannel);
		void begin();
		byte readEEPROM(int address);
		void readEEPROMBlock(int address, byte maxLength, boolean trimmed, char textBlock[]);
		void readBlock (int blockNumber, byte maxLength, bool trimmed, char textBlock[]);
		void writeEEPROM(int address, byte val);
		void writeEEPROMFile ();

	private:
		int _mainI2CChannel;
		int _EEPROMI2CChannel;
		int _EEPROMI2CAddress;
		byte _csPin;
		void EEPROMTCA9548A(uint8_t bus);
};

#endif
