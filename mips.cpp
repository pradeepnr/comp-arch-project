/*
 * mips.cpp
 *
 *  Created on: 12-Nov-2016
 *      Author: pradeepnr
 */
#ifndef MIPS_CPP_
#define MIPS_CPP_
#include "types.h"
#include "mips.h"
#include <iostream>
#include <cmath>
#include <sstream>

namespace MIPS {

ADDI::ADDI(UINT32 val, INT32 pc) : Instruction(val, pc) {
	regS = (val & (FIVEONES << 21))>>21;
	regT = (val & (FIVEONES << 16))>>16;
	imm = (val) & SIXTEENONES;

	std::stringstream sstr;
	sstr<<"ADDI ";
	sstr<<"R"<<regT<<", ";
	sstr<<"R"<<regS<<", ";
	sstr<<"#"<<imm;
#if LOG
	sstr<<" ["<<mPC<<"]";
#endif
	printStr = sstr.str();
}

ADD::ADD(UINT32 val, INT32 pc) : Instruction(val, pc) {
	regS = (val & (FIVEONES << 21))>>21;
	regT = (val & (FIVEONES << 16))>>16;
	regD = (val & (FIVEONES << 11))>>11;

	std::stringstream sstr;
	sstr<<"ADD ";
	sstr<<"R"<<regD<<", ";
	sstr<<"R"<<regS<<", ";
	sstr<<"R"<<regT;
#if LOG
	sstr<<" ["<<mPC<<"]";
#endif
	printStr = sstr.str();
}

ADDU::ADDU(UINT32 val, INT32 pc) : Instruction(val, pc) {
	regS = (val & (FIVEONES << 21))>>21;
	regT = (val & (FIVEONES << 16))>>16;
	regD = (val & (FIVEONES << 11))>>11;

	std::stringstream sstr;
	sstr<<"ADDU ";
	sstr<<"R"<<regD<<", ";
	sstr<<"R"<<regS<<", ";
	sstr<<"R"<<regT;
	printStr = sstr.str();
}

SW::SW(UINT32 val, INT32 pc) : Instruction(val, pc), mCycle(0) {
	base = (val & (FIVEONES << 21))>>21;
	regT = (val & (FIVEONES << 16))>>16;
	offset = (val) & SIXTEENONES;

	std::stringstream sstr;
	sstr<<"SW ";
	sstr<<"R"<<regT<<", ";
	sstr<<offset<<"(";
	sstr<<"R"<<base<<")";
#if LOG
	sstr<<" ["<<mPC<<"]";
#endif
	printStr = sstr.str();
}

LW::LW(UINT32 val, INT32 pc) : Instruction(val, pc), mCycle(0) {
	base = (val & (FIVEONES << 21))>>21;
	regT = (val & (FIVEONES << 16))>>16;
	offset = (val) & SIXTEENONES;

	std::stringstream sstr;
	sstr<<"LW ";
	sstr<<"R"<<regT<<", ";
	sstr<<offset<<"(";
	sstr<<"R"<<base<<")";
#if LOG
	sstr<<" ["<<mPC<<"]";
#endif
	printStr = sstr.str();
}

J::J(UINT32 val, INT32 pc) : Instruction(val, pc) {
	target = (val & TWENTYSIXONES);
	target <<=2;
	std::stringstream sstr;
	sstr<<"J ";
	sstr<<"#"<<target;
#if LOG
	sstr<<" ["<<mPC<<"]";
#endif
	printStr = sstr.str();
}

BEQ::BEQ(UINT32 val, INT32 pc) : Instruction(val, pc) {
	regS = (val & (FIVEONES << 21))>>21;
	regT = (val & (FIVEONES << 16))>>16;
	offset = (val) & SIXTEENONES;

	// This is added as per the MIPS manual
	//TODO The manual, mips.pdf page 70, also talks about 18 bit signed offset
	offset <<= 2;

	std::stringstream sstr;
	sstr<<"BEQ ";
	sstr<<"R"<<regS<<", ";
	sstr<<"R"<<regT<<", ";
	sstr<<"#"<<offset;
#if LOG
	sstr<<" ["<<mPC<<"]";
#endif
	printStr = sstr.str();
}

BNE::BNE(UINT32 val, INT32 pc) : Instruction(val, pc) {
	regS = (val & (FIVEONES << 21))>>21;
	regT = (val & (FIVEONES << 16))>>16;
	offset = (val) & SIXTEENONES;
	offset <<=2;

	std::stringstream sstr;
	sstr<<"BNE ";
	sstr<<"R"<<regS<<", ";
	sstr<<"R"<<regT<<", ";
	sstr<<"#"<<offset;
	printStr = sstr.str();
}

BGEZ::BGEZ(UINT32 val, INT32 pc) : Instruction(val, pc) {
	regS = (val & (FIVEONES << 21))>>21;
	offset = (val) & SIXTEENONES;
	offset <<=2;

	std::stringstream sstr;
	sstr<<"BGEZ ";
	sstr<<"R"<<regS<<", ";
	sstr<<"#"<<offset;
	printStr = sstr.str();
}

BGTZ::BGTZ(UINT32 val, INT32 pc) : Instruction(val, pc) {
	regS = (val & (FIVEONES << 21))>>21;
	offset = (val) & SIXTEENONES;
	offset <<=2;

	std::stringstream sstr;
	sstr<<"BGTZ ";
	sstr<<"R"<<regS<<", ";
	sstr<<"#"<<offset;
	printStr = sstr.str();
}

BLEZ::BLEZ(UINT32 val, INT32 pc) : Instruction(val, pc) {
	regS = (val & (FIVEONES << 21))>>21;
	offset = (val) & SIXTEENONES;
	offset <<=2;

	std::stringstream sstr;
	sstr<<"BLEZ ";
	sstr<<"R"<<regS<<", ";
	sstr<<"#"<<offset;
	printStr = sstr.str();
}

BLTZ::BLTZ(UINT32 val, INT32 pc) : Instruction(val, pc) {
	regS = (val & (FIVEONES << 21))>>21;
	offset = (val) & SIXTEENONES;
	offset <<=2;

	std::stringstream sstr;
	sstr<<"BLTZ ";
	sstr<<"R"<<regS<<", ";
	sstr<<"#"<<offset;
	printStr = sstr.str();
}

ADDIU::ADDIU(UINT32 val, INT32 pc) : Instruction(val, pc) {
	regS = (val & (FIVEONES << 21))>>21;
	regT = (val & (FIVEONES << 16))>>16;
	imm = (val) & SIXTEENONES;

	std::stringstream sstr;
	sstr<<"ADDIU ";
	sstr<<"R"<<regT<<", ";
	sstr<<"R"<<regS<<", ";
	sstr<<"#"<<imm;
	printStr = sstr.str();
}

BREAK::BREAK(UINT32 val, INT32 pc) : Instruction(val, pc) {
	std::stringstream sstr;
	sstr<<"BREAK";
	printStr = sstr.str();
}

SLT::SLT(UINT32 val, INT32 pc) : Instruction(val, pc) {
	regS = (val & (FIVEONES << 21))>>21;
	regT = (val & (FIVEONES << 16))>>16;
	regD = (val & (FIVEONES << 11))>>11;

	std::stringstream sstr;
	sstr<<"SLT ";
	sstr<<"R"<<regD<<", ";
	sstr<<"R"<<regS<<", ";
	sstr<<"R"<<regT;
	printStr = sstr.str();
}

SLTI::SLTI(UINT32 val, INT32 pc) : Instruction(val, pc) {
	regS = (val & (FIVEONES << 21))>>21;
	regT = (val & (FIVEONES << 16))>>16;
	imm = (val) & SIXTEENONES;

	std::stringstream sstr;
	sstr<<"SLTI ";
	sstr<<"R"<<regT<<", ";
	sstr<<"R"<<regS<<", ";
	sstr<<"#"<<imm;
	printStr = sstr.str();

}

SLTU::SLTU(UINT32 val, INT32 pc) : Instruction(val, pc) {
	regS = (val & (FIVEONES << 21))>>21;
	regT = (val & (FIVEONES << 16))>>16;
	regD = (val & (FIVEONES << 11))>>11;

	std::stringstream sstr;
	sstr<<"SLTU ";
	sstr<<"R"<<regD<<", ";
	sstr<<"R"<<regS<<", ";
	sstr<<"R"<<regT;
	printStr = sstr.str();
}

SLL::SLL(UINT32 val, INT32 pc) : Instruction(val, pc) {
	regT = (val & (FIVEONES << 16))>>16;
	regD = (val & (FIVEONES << 11))>>11;
	sa = (val & (FIVEONES << 6))>>6;

	std::stringstream sstr;
	sstr<<"SLL ";
	sstr<<"R"<<regD<<", ";
	sstr<<"R"<<regT<<", ";
	sstr<<"#"<<sa;
	printStr = sstr.str();
}

SRL::SRL(UINT32 val, INT32 pc) : Instruction(val, pc) {
	regT = (val & (FIVEONES << 16))>>16;
	regD = (val & (FIVEONES << 11))>>11;
	sa = (val & (FIVEONES << 6))>>6;


	std::stringstream sstr;
	sstr<<"SRL ";
	sstr<<"R"<<regD<<", ";
	sstr<<"R"<<regT<<", ";
	sstr<<"#"<<sa;
	printStr = sstr.str();
}

SRA::SRA(UINT32 val, INT32 pc) : Instruction(val, pc) {
	regT = (val & (FIVEONES << 16))>>16;
	regD = (val & (FIVEONES << 11))>>11;
	sa = (val & (FIVEONES << 6))>>6;


	std::stringstream sstr;
	sstr<<"SRA ";
	sstr<<"R"<<regD<<", ";
	sstr<<"R"<<regT<<", ";
	sstr<<"#"<<sa;
	printStr = sstr.str();
}

SUB::SUB(UINT32 val, INT32 pc) : Instruction(val, pc) {
	regS = (val & (FIVEONES << 21))>>21;
	regT = (val & (FIVEONES << 16))>>16;
	regD = (val & (FIVEONES << 11))>>11;

	std::stringstream sstr;
	sstr<<"SUB ";
	sstr<<"R"<<regD<<", ";
	sstr<<"R"<<regS<<", ";
	sstr<<"R"<<regT;
	printStr = sstr.str();
}

SUBU::SUBU(UINT32 val, INT32 pc) : Instruction(val, pc) {
	regS = (val & (FIVEONES << 21))>>21;
	regT = (val & (FIVEONES << 16))>>16;
	regD = (val & (FIVEONES << 11))>>11;

	std::stringstream sstr;
	sstr<<"SUBU ";
	sstr<<"R"<<regD<<", ";
	sstr<<"R"<<regS<<", ";
	sstr<<"R"<<regT;
	printStr = sstr.str();
}

AND::AND(UINT32 val, INT32 pc) : Instruction(val, pc) {
	regS = (val & (FIVEONES << 21))>>21;
	regT = (val & (FIVEONES << 16))>>16;
	regD = (val & (FIVEONES << 11))>>11;

	std::stringstream sstr;
	sstr<<"AND ";
	sstr<<"R"<<regD<<", ";
	sstr<<"R"<<regS<<", ";
	sstr<<"R"<<regT;
	printStr = sstr.str();
}

OR::OR(UINT32 val, INT32 pc) : Instruction(val, pc) {
	regS = (val & (FIVEONES << 21))>>21;
	regT = (val & (FIVEONES << 16))>>16;
	regD = (val & (FIVEONES << 11))>>11;

	std::stringstream sstr;
	sstr<<"OR ";
	sstr<<"R"<<regD<<", ";
	sstr<<"R"<<regS<<", ";
	sstr<<"R"<<regT;
	printStr = sstr.str();
}

XOR::XOR(UINT32 val, INT32 pc) : Instruction(val, pc) {
	regS = (val & (FIVEONES << 21))>>21;
	regT = (val & (FIVEONES << 16))>>16;
	regD = (val & (FIVEONES << 11))>>11;

	std::stringstream sstr;
	sstr<<"XOR ";
	sstr<<"R"<<regD<<", ";
	sstr<<"R"<<regS<<", ";
	sstr<<"R"<<regT;
	printStr = sstr.str();
}

NOR::NOR(UINT32 val, INT32 pc) : Instruction(val, pc) {
	regS = (val & (FIVEONES << 21))>>21;
	regT = (val & (FIVEONES << 16))>>16;
	regD = (val & (FIVEONES << 11))>>11;

	std::stringstream sstr;
	sstr<<"NOR ";
	sstr<<"R"<<regD<<", ";
	sstr<<"R"<<regS<<", ";
	sstr<<"R"<<regT;
	printStr = sstr.str();
}

NOP::NOP(UINT32 val, INT32 pc) : Instruction(val, pc) {
	printStr = std::string("NOP");
}

}

#endif //MIPS_CPP_




