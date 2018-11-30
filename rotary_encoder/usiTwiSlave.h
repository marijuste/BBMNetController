/*##################################################################################################
	Name	: USI TWI Slave driver - I2C/TWI-EEPROM
	Version	: 1.2  - stabil running
	autor	: Martin Junghans	jtronics@gmx.de
	page	: www.jtronics.de
	License	: GNU General Public License 

	Created from Atmel source files for Application Note AVR312: Using the USI Modulea 
	as an I2C slave like a I2C-EEPROM.

	LICENSE:    Copyright (C) 2010 Marin Junghans

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
	
//################################################################################################## 
	Description
	
	The Slave works like a I2C-EEPROM.
	
		1. 	send - "SLave-Address + write"
		2. 	send - "Buffer-Address" 		Address in which you like to start with write or read.
		
		write data in to Slave rxbuffer
		3. 	send - "SLave-Address + write"  
		4.	send - "data"					writes data in the buffer, start by rxbuffer[Buffer-Address]

		or 
	
		read data from Slave txbuffer
		3. 	send - "SLave-Address + read" 	
		4.	send - data = i2c_readAck();	Demands the Slave to send data, start by txbuffer[Buffer-Address].
		
		Info:
		- you have to change the buffer_size in the usiTwiSlave.h file
		- Buffer-Address is counted up automatically
		- if Buffer-Address > buffersize --> start by Buffer-Address= 0
	
//################################################################################################*/

#ifndef _USI_TWI_SLAVE_H_
#define _USI_TWI_SLAVE_H_

//################################################ includes
#include <stdbool.h>

//################################################ prototypes
void    usiTwiSlaveInit(uint8_t ownAddress);	// send Slaveadresse

//################################################ variablen

#define buffer_size 6						//in Byte (2..254)	CHANGE only here!!!!!


volatile uint8_t rxbuffer[buffer_size];
volatile uint8_t txbuffer[buffer_size];			// Der Sendebuffer, der vom Master ausgelesen werden kann.
volatile uint8_t buffer_adr; 					// "Adressregister" für den Buffer


#if 	(buffer_size > 254)
		#error Buffer to big! Maximal 254 Bytes.
		
#elif 	(buffer_size < 2)
		#error Buffer to small! mindestens 2 Bytes!
#endif

//################################################
#endif  // ifndef _USI_TWI_SLAVE_H_
