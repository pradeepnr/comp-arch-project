/*
 * disassembler.cpp
 *
 *  Created on: 12-Nov-2016
 *      Author: pradeepnr
 */
#ifndef DISASSEMBLER_CPP_
#define DISASSEMBLER_CPP_

#include <iostream>
//#include <bitset>
#include <vector>
#include "disassembler.h"
#include "mips.h"
#define BASE_ADDR 600

typedef std::bitset<8> BIT8;
typedef std::bitset<32> BIT32;
typedef std::vector<BIT8> VBITS;

using MIPS::Instruction;

Disassembler::Disassembler(string ipfilename, string opfilename)
		: mIPFilename(ipfilename),
		  mOPFilename(opfilename),
		  mIFile(NULL),
		  mOFile(NULL){
	// check if file exist then
	mIFile = new ifstream(mIPFilename, ifstream::binary) ;
	mOFile = new ofstream(mOPFilename, ofstream::binary) ;
	mPc = 0;
}

Disassembler::~Disassembler() {
	if(mIFile) {
		delete mIFile;
	}
	if(mOFile) {
		delete mOFile;
	}
}

void Disassembler::run() {
//	cout<<"Disassembler::run()\n";
#if 1
	bool stop = false;
	UINT32 val = 0;
	UINT32 buf;
	while(mIFile->read((char*)&buf,4) && !mIFile->eof()) {
		val = buf;
		if(isLittleEndian()) {
			val = swapUINT32(buf);
		}

		if(!stop) {
			printInputInstruction(val);
			(*mOFile)<<" ";
			//print the binary
			Instruction* inst = InstructionSet::decode(val, mPc);
			if(inst) {
				(*mOFile)<<(BASE_ADDR + mPc)<<" "<<inst->getPrintStr()<<endl;
				if(inst->type() == MIPS::OPBREAK) {
					stop = true;
				}
				delete inst;
			} else {
				(*mOFile)<<"Could not recognize the instruction\n";
			}
		}
		else {
			bitset<32> read(val);
			(*mOFile)<<read<<" "<<(BASE_ADDR + mPc)<<" "<<val<<endl;
		}
		mPc +=4;
	}
#else
	mPc = 60;
	UINT32 val = readU32();
	Instruction* inst = mInstSet.decode(val);
	if(inst) {
		cout<<(BASE_ADDR + mPc)<<" "<<inst->getPrintStr()<<endl;
	} else {
		cout<<"Could not recognize the instruction\n";
	}
	delete inst;
#endif
}

bool Disassembler::isLittleEndian() {
	unsigned int fourBytes = 1;
	char oneByte = ((char*)&fourBytes)[0];
	if(oneByte == 1)
		return true;
	return false;
}

void Disassembler::printInputInstruction(UINT32 val) {
	bitset<32> read(val);
	for(int i=31;i>=0;--i) {
		if(i==5 || i==10 || i==15 || i==20 || i==25 ) {
			(*mOFile)<<" ";
		}
		(*mOFile)<<read[i];
	}
}

//001000 00000 01000 00000 00000 101100

void Disassembler::fetchInstruction() {

}

UINT32 Disassembler::readU32() {
	UINT32 val = 0;
	mIFile->seekg(mPc);
	UINT32 buf;
	mIFile->read((char*)&buf,4);
	val = swapUINT32(buf);
	return val;
}
void Disassembler::advancePC() {

}

UINT32 Disassembler::swapUINT32( UINT32 val )
{
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0x00FF00FF );
    return (val << 16) | (val >> 16);
}

#endif // DISASSEMBLER_CPP_




