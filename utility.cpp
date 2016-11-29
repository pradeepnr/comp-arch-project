/*
 * utility.cpp
 *
 *  Created on: 16-Nov-2016
 *      Author: pradeepnr
 */

#include "utility.h"

bool Utility::isLittleEndian() {
	unsigned int fourBytes = 1;
	char oneByte = ((char*)&fourBytes)[0];
	if(oneByte == 1)
		return true;
	return false;
}

UINT32 Utility::swapUINT32( UINT32 val )
{
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0x00FF00FF );
    return (val << 16) | (val >> 16);
}


