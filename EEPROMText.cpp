/*
 EEPROMText.h - Library for reading or writing texts from or to an EEPROM
 Created by Adri Timmermans, September 24, 2020
 Private release
 
 prerequisites: I2C bus is activated befor first call to a read/write action
 
*/
#include "Arduino.h"
#include "EEPROMText.h"
#include "Wire.h"
#include <SPI.h>
#include <SD.h>

EEPROMText::EEPROMText (int I2CChannel, byte address, byte csPin, int returnChannel)
{
  _mainI2CChannel   = returnChannel;
  _EEPROMI2CChannel = I2CChannel;
  _EEPROMI2CAddress = address;
  _csPin            = csPin;
}
void EEPROMText::EEPROMTCA9548A(uint8_t bus)
{
	Wire.beginTransmission(0x70);
	Wire.write(1 << bus);
	Wire.endTransmission();
}

void EEPROMText::begin()
{
}

byte EEPROMText::readEEPROM(int address)
{
    // Define byte for received data
    byte rcvData = 0xFF;

	// Switch to I2C bus connected to EEPROM
	
	EEPROMTCA9548A(_EEPROMI2CChannel);

    // Begin transmission to I2C EEPROM
	
    Wire.beginTransmission(_EEPROMI2CAddress);

    // Send memory address as two 8-bit bytes
    Wire.write((int)(address >> 8));   // MSB
    Wire.write((int)(address & 0xFF)); // LSB

    // End the transmission
    Wire.endTransmission();

    // Request one byte of data at current memory address
    Wire.requestFrom(_EEPROMI2CAddress, 1);

    // Read the data and assign to variable
    rcvData =  Wire.read();
      // Return the data as function output
	EEPROMTCA9548A(_mainI2CChannel);
    return rcvData;
}
  
void   EEPROMText::readEEPROMBlock(int address, byte maxLength, boolean trimmed, char textBlock[])
{
	int pointer;
	int i;
	byte tst;
	boolean blockReceived = false;
	//sprintf(textBlock, "--%d, %d --", address, maxLength);
	
	while (!blockReceived)
	{
	// Switch to I2C bus connected to EEPROM
	
		EEPROMTCA9548A(_EEPROMI2CChannel);
		
		Wire.beginTransmission(_EEPROMI2CAddress);

		// Send memory address as two 8-bit bytes
		Wire.write((int)(address >> 8));   // MSB
		Wire.write((int)(address & 0xFF)); // LSB

		// End the transmission
		Wire.endTransmission();
		delay(5);
		// Request one byte of data at current memory address
		Wire.requestFrom(_EEPROMI2CAddress, (int)maxLength);

		// Read the data and assign to variable
		i=0;
		while(Wire.available())
		{
			tst = Wire.read();
			textBlock[i] = char(tst);
			i++;
		}
		blockReceived = (i == maxLength);
		/*
		if (!blockReceived)
		{
			Serial.println("");
			Serial.println("block not according to specs");
		}
		*/
	}
	pointer = i;
	textBlock[maxLength] = 0;
    if (trimmed)
    {
		pointer--;
        while (pointer > 0)
        {
			if (textBlock[pointer] != 32 )
			{
				textBlock[pointer + 1] = 0;
				pointer = 1;
			}
			pointer--;
		}
    }
	else
	{
		textBlock[pointer] = 0;
	}
	EEPROMTCA9548A(_mainI2CChannel);
}

void EEPROMText::readBlock (int blockNumber, byte maxLength, bool trimmed, char textBlock[])
{

	byte tst;
	char textBlockControl[33];
    unsigned int EEPROMAddress;
    byte pointer = 0;
    EEPROMAddress = (blockNumber - 1) * 32 + 1;
	readEEPROMBlock(EEPROMAddress, maxLength, trimmed, textBlock);
	delay(6);
/*	readEEPROMBlock(EEPROMAddress, maxLength, trimmed, textBlockControl);
	if (strcmp(textBlock, textBlockControl) != 0)
	{
		readEEPROMBlock(EEPROMAddress, maxLength, trimmed, textBlock);		
	}
*/	
}

void EEPROMText::writeEEPROM(int address, byte val)
{
    byte MSB, LSB;
		// Switch to I2C bus connected to EEPROM
	
	EEPROMTCA9548A(_EEPROMI2CChannel);

    // Begin transmission to I2C EEPROM
    Wire.beginTransmission(_EEPROMI2CAddress);

    // Send memory address as two 8-bit bytes
    MSB = (address & 0xFF00)>>8;
    LSB = (address & 0x00FF);
    
    Wire.write((int)(address >> 8));   // MSB
    Wire.write((int)(address & 0xFF)); // LSB
    // Send data to be stored
    Wire.write(val);
    // End the transmission
    Wire.endTransmission();

    // Add 5ms delay for EEPROM
    delay(5);
	EEPROMTCA9548A(_mainI2CChannel);
}

void EEPROMText::writeEEPROMFile()
{
  File myFile;
  
  if (!SD.begin(_csPin)) 
  {
    return;
  }
  EEPROMTCA9548A(_EEPROMI2CChannel);

// Connect to I2C bus as master
  myFile = SD.open("tbstrip.txt");
  if (myFile) 
  {
    unsigned int eePROMAddress = 0;
    byte stringLength = 0;
    byte oneChar;
    // read from the file until there's nothing else in it:
    while (myFile.available()) 
    {
		oneChar = myFile.read();
		if ((oneChar != 13) && (oneChar != 10))
		{
		  stringLength++;
		  eePROMAddress++;
		  writeEEPROM(eePROMAddress, oneChar);
		}
		else
		{
			if (oneChar != 10)
			{
				Serial.print(" - length = ");
				Serial.println (stringLength);
				for (int i = stringLength + 1; i <= 32; i++)
				{
					eePROMAddress++;
					writeEEPROM(eePROMAddress, 0x20);
				}
				stringLength = 0;
			}
		}
    }
    // close the file:
    myFile.close();
  }
  EEPROMTCA9548A(_mainI2CChannel);
}
