/*
 * instructionSet.h
 *
 *  Created on: 12-Nov-2016
 *      Author: pradeepnr
 */

#ifndef INSTRUCTIONSET_H_
#define INSTRUCTIONSET_H_
#include "types.h"
#include "mips.h"


class InstructionSet {
public:
	static MIPS::Instruction* decode(UINT32 val, INT32 pc);
	static MIPS::Instruction* getType2(UINT32 val, INT32 pc);
	static MIPS::Instruction* getSpecialType(UINT32 val, INT32 pc);
	static bool IsBranch(MIPS::Instruction*);
	static bool IsLoad(MIPS::Instruction*);
	static bool IsStore(MIPS::Instruction*);
};

enum OpCodeType1 {
	OpCodeSW 	= 0x2B,
	OpCodeLW 	= 0x23,
	OpCodeJ 	= 0x2,
	OpCodeBEQ 	= 0x4,
	OpCodeBNE 	= 0x5,
	OpCodeBGTZ 	= 0x7,
	OpCodeBLEZ 	= 0x6,
	OpCodeADDI 	= 0x08,
	OpCodeADDIU = 0x09,
	OpCodeSLTI 	= 0x0A,
	TYPE2 		= 0x1,
	SPECIAL		= 0x0
};

enum OpCodeType2 {
	OpCodeBGEZ = 0x01,
	OpCodeBLTZ = 0x0
};

enum OpCodeSpecial {
	OpCodeBREAK = 0xD,
	OpCodeSLT 	= 0x2A,
	OpCodeSLTU	= 0x2B,
	OpCodeSLL 	= 0x0,
	OpCodeSRL 	= 0x2,
	OpCodeSRA 	= 0x3,
	OpCodeADD 	= 0x20,
	OpCodeADDU 	= 0x21,
	OpCodeSUB 	= 0x22,
	OpCodeSUBU 	= 0x23,
	OpCodeAND 	= 0x24,
	OpCodeOR 	= 0x25,
	OpCodeXOR 	= 0x26,
	OpCodeNOR 	= 0x27,
};

#endif //INSTRUCTIONSET_H_

