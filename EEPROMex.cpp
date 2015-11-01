/*
  EEPROMEx.cpp - Extended EEPROM library
  Copyright (c) 2012 Thijs Elenbaas.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/******************************************************************************
 * Includes
 ******************************************************************************/
#include "EEPROMex.h"

/******************************************************************************
 * Definitions
 ******************************************************************************/

#define _EEPROMEX_VERSION 2_0_0 // software version of this library


/******************************************************************************
 * Constructors
 ******************************************************************************/

//EEPROMClassEx::EEPROMClassEx(){}

/******************************************************************************
 * User API
 ******************************************************************************/

 /**
 * Set starting position and memory size that EEPROMEx may manage
 */
void EEPROMClassEx::setMemPool(int base, int memSize) {
	//Base can only be adjusted if no addresses have already been issued
	if (_nextAvailableaddress == _base) 
		_base = base;
		_nextAvailableaddress=_base;
	
	//Ceiling can only be adjusted if not below issued addresses
	if (memSize >= _nextAvailableaddress ) 
		_memSize = memSize;
}


/**
 * Get a new starting address to write to. Adress is negative if not enough space is available
 */
int EEPROMClassEx::getAddress(int noOfBytess){
	int availableaddress   = _nextAvailableaddress;
	_nextAvailableaddress += noOfBytess;
	return availableaddress;		
}
 
/**
 * Check if EEPROM memory is ready to be accessed
 */
bool EEPROMClassEx::isReady() {
	//return eeprom_is_ready();
	return true;
}

/**
 * Read a single uint8_t
 * This function performs as readByte and is added to be similar to the EEPROM library
 */
uint8_t EEPROMClassEx::read(int address)
{
	return readByte(address);
}

/**
 * Read a single bit
 */
bool EEPROMClassEx::readBit(int address, uint8_t bit) {
	  if (bit> 7) return false; 
	  if (!isReadOk(address+sizeof(uint8_t))) return false;
	  uint8_t BytesVal =  EEPROM.read(address); //eeprom_read_byte((unsigned char *) address);      
	  uint8_t BytesPos = (1 << bit);
      return (BytesVal & BytesPos);
}

/**
 * Read a single uint8_t
 */
uint8_t EEPROMClassEx::readByte(int address)
{	
	if (!isReadOk(address+sizeof(uint8_t))) return 0;
	uint8_t b = EEPROM.read(address);
	return EEPROM.read(address); //eeprom_read_byte((unsigned char *) address);
}

/**
 * Read a single 16 bits integer
 */
uint16_t EEPROMClassEx::readInt(int address)
{
	if (!isReadOk(address+sizeof(uint16_t))) return 0;
	//return eeprom_read_word((uint16_t *) address);
	union uint16_t_bytes {
       uint16_t val;
       uint8_t  bytes[sizeof(uint16_t)];
    } data;
  for(int i=0; i<sizeof(uint16_t); i++){
  	data.bytes[i] = EEPROM.read(address+i);  	
  }
	return data.val;
}

/**
 * Read a single 32 bits integer
 */
uint32_t EEPROMClassEx::readLong(int address)
{
	if (!isReadOk(address+sizeof(uint32_t))) return 0;
	//return eeprom_read_dword((unsigned long *) address);
	union uint32_t_bytes {
       uint32_t val;
       uint8_t  bytes[sizeof(uint32_t)];
    } data;
  for(int i=0; i<sizeof(uint32_t); i++){
  	data.bytes[i] = EEPROM.read(address+i);  	
  }
	return data.val;	
}

/**
 * Read a single float value
 */
float EEPROMClassEx::readFloat(int address)
{
	if (!isReadOk(address+sizeof(float))) return 0;
	float _value;
	readBlock<float>(address, _value);
	return _value;
}

/**
 * Read a single double value (size will depend on board type)
 */
double EEPROMClassEx::readDouble(int address)
{
	if (!isReadOk(address+sizeof(double))) return 0;	
	double _value;
	readBlock<double>(address, _value);
	return _value;
}

/**
 * Write a single uint8_t
 * This function performs as writeByte and is added to be similar to the EEPROM library
 */
bool EEPROMClassEx::write(int address, uint8_t value)
{
	return writeByte(address, value);
}

/**
 * Write a single bit
 */
bool EEPROMClassEx::writeBit(int address, uint8_t bit, bool value) {
	updateBit(address, bit, value);
	return true;
}

/**
 * Write a single uint8_t
 */
bool EEPROMClassEx::writeByte(int address, uint8_t value)
{
	if (!isWriteOk(address+sizeof(uint8_t))) return false;
	EEPROM.write(address, value); //eeprom_write_byte((unsigned char *) address, value);
	EEPROM.commit();
	return true;
}

/**
 * Write a single 16 bits integer
 */
bool EEPROMClassEx::writeInt(int address, uint16_t value)
{
	if (!isWriteOk(address+sizeof(uint16_t))) return false;
	//eeprom_write_word((uint16_t *) address, value);
	union uint16_t_bytes {
       uint16_t val;
       uint8_t  bytes[sizeof(uint16_t)];
    } data;
  data.val = value;
  for(int i=0; i<sizeof(uint16_t); i++)
  	EEPROM.write(address+i, data.bytes[i]);  
  EEPROM.commit();  	
	return true;
}

/**
 * Write a single 32 bits integer
 */
bool EEPROMClassEx::writeLong(int address, uint32_t value)
{
	if (!isWriteOk(address+sizeof(uint32_t))) return false;
	//eeprom_write_dword((unsigned long *) address, value);
	union uint32_t_bytes {
       uint32_t val;
       uint8_t  bytes[sizeof(uint32_t)];
    } data;
  data.val = value;
  for(int i=0; i<sizeof(uint32_t); i++){
  	EEPROM.write(address+i, data.bytes[i]);
  }
  EEPROM.commit();  	
	return true;
}

/**
 * Write a single float value
 */
bool EEPROMClassEx::writeFloat(int address, float value)
{
	return (writeBlock<float>(address, value)!=0);	
}

/**
 * Write a single double value (size will depend on board type)
 */
bool EEPROMClassEx::writeDouble(int address, double value)
{
	return (writeBlock<float>(address, value)!=0);	
}

/**
 * Update a single uint8_t
 * The EEPROM will only be overwritten if different. This will reduce wear.
 * This function performs as updateuint8_t and is added to be similar to the EEPROM library
 */
bool EEPROMClassEx::update(int address, uint8_t value)
{
	return (updateBytes(address, value));
}

/**
 * Update a single bit
 * The EEPROM will only be overwritten if different. This will reduce wear.
 */
bool EEPROMClassEx::updateBit(int address, uint8_t bit, bool value) 
{
	  if (bit> 7) return false; 
	  
	  uint8_t BytesValInput  = readByte(address);
	  uint8_t BytesValOutput = BytesValInput;	  
	  // Set bit
	  if (value) {	    
		BytesValOutput |= (1 << bit);  //Set bit to 1
	  } else {		
	    BytesValOutput &= ~(1 << bit); //Set bit to 0
	  }
	  // Store if different from input
	  if (BytesValOutput!=BytesValInput) {
		writeByte(address, BytesValOutput);	  
	  }
	  return true;
}


/**
 * Update a single uint8_t
 * The EEPROM will only be overwritten if different. This will reduce wear.
 */
bool EEPROMClassEx::updateBytes(int address, uint8_t value)
{
	return (updateBlock<uint8_t>(address, value)!=0);
}

/**
 * Update a single 16 bits integer 
 * The EEPROM will only be overwritten if different. This will reduce wear.
 */
bool EEPROMClassEx::updateInt(int address, uint16_t value)
{
	return (updateBlock<uint16_t>(address, value)!=0);
}

/**
 * Update a single 32 bits integer 
 * The EEPROM will only be overwritten if different. This will reduce wear.
 */
bool EEPROMClassEx::updateLong(int address, uint32_t value)
{
	return (updateBlock<uint32_t>(address, value)!=0);
}

/**
 * Update a single float value 
 * The EEPROM will only be overwritten if different. This will reduce wear.
 */
bool EEPROMClassEx::updateFloat(int address, float value)
{
	return (updateBlock<float>(address, value)!=0);
}

/**
 * Update a single double value (size will depend on board type)
 * The EEPROM will only be overwritten if different. This will reduce wear.
 */
bool EEPROMClassEx::updateDouble(int address, double value)
{
	return (writeBlock<double>(address, value)!=0);
}

/**
 * Performs check to see if writing to a memory address is allowed
 */
bool EEPROMClassEx::isWriteOk(int address)
{
	return true;
}

/**
 * Performs check to see if reading from a memory address is allowed
 */
bool EEPROMClassEx::isReadOk(int address)
{
	return true;	
}

int EEPROMClassEx::_base= 0;
int EEPROMClassEx::_memSize= 512;
int EEPROMClassEx::_nextAvailableaddress= 0;
int EEPROMClassEx::_writeCounts =0;

EEPROMClassEx EEPROMEx;
