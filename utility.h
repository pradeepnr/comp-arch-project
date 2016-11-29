/*
 * utility.h
 *
 *  Created on: 16-Nov-2016
 *      Author: pradeepnr
 */

#ifndef UTILITY_H_
#define UTILITY_H_
#include "types.h"

class Utility {
public:
	static bool isLittleEndian();
	static UINT32 swapUINT32( UINT32 val );
};



#endif /* UTILITY_H_ */
