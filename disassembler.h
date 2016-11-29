/*
 * disassembler.h
 *
 *  Created on: 12-Nov-2016
 *      Author: pradeepnr
 */

#ifndef DISASSEMBLER_H_
#define DISASSEMBLER_H_

#include <string>
#include <fstream>
#include "types.h"
#include "instructionSet.h"

using namespace std;

class Disassembler {
public:
	//constructor and set filename, init istream, pc
	Disassembler(string ipfilename, string opfilename);
	~Disassembler();

	// should read instruction at pc and decode the values
	void run();

private:
	void fetchInstruction();
	UINT32 readU32();
	void advancePC();
	//to conver from big endian to little endian
	UINT32 swapUINT32( UINT32 val );
	bool isLittleEndian();
	void printInputInstruction(UINT32 val);

	string mIPFilename;
	string mOPFilename;
	istream* mIFile;
	ofstream* mOFile;
	INT32 mPc;
};

//00100000 00001000 00000000 00101100
//001000 00000 01000 00000 00000 101100
//ADDI $0 $8

#endif //DISASSEMBLER_H_

#if 0
//	mFile->read( (char*)bytes, 4 );  // read 2 bytes from the file
//
//	BIT32 res;
//	res.reset();
//	VBITS vec;
//
//	for(int i=0;i<4;++i) {
//		vec.push_back(BIT8(bytes[i]));
//	}
//	for(int i=3;i>=0;--i) {
//		for(int j=0;j<=7;++j) {
//			res[31- ((i*8) + (7-j))] = vec[i][j];
//		}
//	}
//
//	val = static_cast<UINT32>(res.to_ulong());

#endif
