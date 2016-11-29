/*
 * processor.h
 *
 *  Created on: 16-Nov-2016
 *      Author: pradeepnr
 */

#ifndef PROCESSOR_H_
#define PROCESSOR_H_

#include <map>
#include <string>
#include <queue>
#include <fstream>
#include "instructionSet.h"
#include "types.h"
#include "buffers.h"

typedef std::pair<UINT32, INT32> PKT;//first->ROB id and second->execution result
enum Dispay { NONE, COMPLETE, PARTIAL, FINAL };
class Processor {
public:
	Processor(std::string ipfilename, std::string opfilename, int m, int n);
	~Processor();
	void init();
	void process();
private:
	void fetch();
	void decode();
	void execute();
	void memory();
	void writeResult();
	void updateRS(PKT& pkt);
	void commit();
	bool isBranchTaken(RSEntry* ent);
	INT32 getBranchOffset(RSEntry* ent);
	INT32 exeInst(RSEntry* ent);
	void printRegs();
	void printMem();
	void printIQ();
	void resetBuffers();
	void printRAT();
	void printREG();
	void DisplayState();

	UINT32 mPC;
	std::string mIPFilename;
	std::string mOPFilename;
	std::istream* mIFile;
	std::ofstream* mOFile;
	std::map<UINT32,UINT32> mMem;
	std::queue<MIPS::Instruction*> mInstQueue;
	BTB mBTB;
	INT32 mReg[32]; // Register File
	INT32 mRAT[32]; // register alias table i=-1 mReg is good, i>=0 read result of ith ROB entry
	ReservationStation mRS;
	std::queue<PKT> mCDB; // common data bus;
	std::queue<RSEntry*> mExeQueue;
	std::queue<RSEntry*> mMemQueue; // load instruction that are ready to access memory after execution is done
	bool mRun;
	ROB mRob;
	bool mPopRob;
	bool mMissPredicted;
	UINT32 mNewPC;
	bool mStopFetch;
	INT32 mStartC;
	INT32 mEndC;
};

#endif /* PROCESSOR_H_ */
