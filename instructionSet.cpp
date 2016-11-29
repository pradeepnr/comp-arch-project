/*
 * instructionSet.cpp
 *
 *  Created on: 12-Nov-2016
 *      Author: pradeepnr
 */
#ifndef INSTRUCTIONSET_CPP_
#define INSTRUCTIONSET_CPP_
#include "types.h"
#include "mips.h"
#include "instructionSet.h"
#include <iostream>


MIPS::Instruction* InstructionSet::decode(UINT32 val, INT32 pc) {
	//std::cout<<"InstructionSet::decode -entry\n";
	if(val == 0x0) {
		return new MIPS::NOP(val, pc);
	}
	UINT32 msbSixBits = (val & 0xFC000000)>>26;
	//std::cout<<"msbSixBits->"<<msbSixBits<<std::endl;
	switch(msbSixBits) {
		case OpCodeSW :
		{
			return new MIPS::SW(val, pc);
		}
		case OpCodeLW :
		{
			return new MIPS::LW(val, pc);
		}
		case OpCodeJ :
		{
			return new MIPS::J(val, pc);
		}
		case OpCodeBEQ :
		{
			return new MIPS::BEQ(val, pc);
		}
		case OpCodeBNE :
		{
			return new MIPS::BNE(val, pc);
		}
		case OpCodeBGTZ :
		{
			return new MIPS::BGTZ(val, pc);
		}
		case OpCodeBLEZ :
		{
			return new MIPS::BLEZ(val, pc);
		}
		case OpCodeADDI :
		{
			return new MIPS::ADDI(val, pc);
		}
		case OpCodeADDIU :
		{
			return new MIPS::ADDIU(val, pc);
		}
		case OpCodeSLTI :
		{
			return new MIPS::SLTI(val, pc);
		}

		case TYPE2 :
		{
			return getType2(val, pc);
		}
		case SPECIAL :
		{
			return getSpecialType(val, pc);
		}

	}
	//std::cout<<"InstructionSet::decode -exit\n";
	return NULL;
}

MIPS::Instruction* InstructionSet::getType2(UINT32 val, INT32 pc) {
	UINT32 type = (val & 0x1F0000)>>16;
	if(type == OpCodeBGEZ) {
		return new MIPS::BGEZ(val, pc);
	}
	else if (type == OpCodeBLTZ) {
		return new MIPS::BLTZ(val, pc);
	}
	return NULL;
}

MIPS::Instruction* InstructionSet::getSpecialType(UINT32 val, INT32 pc) {
	UINT32 type = (val & 0x3F);
	switch(type) {
		case OpCodeBREAK :
		{
			return new MIPS::BREAK(val, pc);
		}
		case OpCodeSLT :
		{
			return new MIPS::SLT(val, pc);
		}
		case OpCodeSLTU :
		{
			return new MIPS::SLTU(val, pc);
		}
		case OpCodeSLL :
		{
			return new MIPS::SLL(val, pc);
		}
		case OpCodeSRL :
		{
			return new MIPS::SRL(val, pc);
		}
		case OpCodeSRA :
		{
			return new MIPS::SRA(val, pc);
		}
		case OpCodeADD :
		{
			return new MIPS::ADD(val, pc);
		}
		case OpCodeADDU :
		{
			return new MIPS::ADDU(val, pc);
		}
		case OpCodeSUB :
		{
			return new MIPS::SUB(val, pc);
		}
		case OpCodeSUBU :
		{
			return new MIPS::SUBU(val, pc);
		}
		case OpCodeAND :
		{
			return new MIPS::AND(val, pc);
		}
		case OpCodeOR :
		{
			return new MIPS::OR(val, pc);
		}
		case OpCodeXOR :
		{
			return new MIPS::XOR(val, pc);
		}
		case OpCodeNOR :
		{
			return new MIPS::NOR(val, pc);
		}
		default :
		{

		}
	}
	return NULL;
}
//, OPJ, OPBEQ, OPBNE, OPBGEZ, OPBGTZ, OPBLEZ, OPBLTZ, OPADDI, OPADDIU, OPBREAK, OPSLT,
//	OPSLTI, OPSLTU, OPSLL, OPSRL, OPSRA, OPSUB, OPSUBU, OPADD, OPADDU, OPAND, OPOR, OPXOR, OPNOR, OPNOP

bool InstructionSet::IsBranch(MIPS::Instruction* inst) {
	if(inst) {
		MIPS::OpCode code = inst->type();
		if(code == MIPS::OPJ || code == MIPS::OPBEQ || code == MIPS::OPBNE|| code == MIPS::OPBGEZ||
				code == MIPS::OPBGTZ|| code == MIPS::OPBLEZ|| code == MIPS::OPBLTZ) {
			return true;
		}
	}
	return false;
}

bool InstructionSet::IsLoad(MIPS::Instruction* inst) {
	if(inst) {
		MIPS::OpCode code = inst->type();
		if(code == MIPS::OPLW) {
			return true;
		}
	}
	return false;
}

bool InstructionSet::IsStore(MIPS::Instruction* inst) {
	if(inst) {
		MIPS::OpCode code = inst->type();
		if(code == MIPS::OPSW) {
			return true;
		}
	}
	return false;
}
#endif //INSTRUCTIONSET_CPP_




