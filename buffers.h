/*
 * reservationStation.h
 *
 *  Created on: 17-Nov-2016
 *      Author: pradeepnr
 */

#ifndef BUFFERS_H_
#define BUFFERS_H_

#include "types.h"
#include "mips.h"
#include <list>
#include <vector>
#include <fstream>

#define RSSIZE 10
#define ROBSIZE 6
#define BTBSIZE 16

class RSEntry {
public:
	bool mReady[2]; // ready to be executed
	UINT32 mParam[2]; // reg value or ROB entry id
	MIPS::Instruction* mInst;
	UINT32 mRobId;
	UINT32 mId;
	bool mExecuted;
	bool exeReady() { return (mReady[0] && mReady[1]); }
};
typedef std::list<RSEntry*> LRSE;
class ReservationStation {
public:
	ReservationStation() : mCurId(0) {}
	bool isRSSlotFree();
	RSEntry* getFreeSlot();
	LRSE::iterator getRSItrBegin() { return mList.begin(); }
	LRSE::iterator getRSItrEnd() { return mList.end(); }
	void releaseEntry(UINT32 rsID);
	void printRS(std::ofstream* ofile);
	void reset();
private:
	LRSE mList;
	UINT32 mCurId;
};

//*********************** ROB *********************

class ROBEntry {
public:
	bool mDone; // computation is done and ready for commit
	INT32 mValue; // computed value,  for LW addr of mem
	INT32 mDestReg; // Destination register to which value will be entered, for LW reg
	UINT32 mId; // ROB entry unique id
	MIPS::Instruction* mInst;
	UINT32 mStoreaddr; // only incase of Store
	UINT32 mRSid;
	bool mComitted;
};

typedef std::list<ROBEntry*> LROBE;
class ROB {
public:
	ROB() : mCurId(0){}
	bool isROBSlotFree();
	ROBEntry* getFreeSlot();
	ROBEntry* getEntry(UINT32 Robid);
	ROBEntry* getCommit();
	void popLastCommit();
	bool isNoEarlyStoreWithAddr(UINT32 robid, UINT32 addr);
	void printRob(std::ofstream* ofile);
	bool hasNOP();
	void reset();
private:
	LROBE mList;
	UINT32 mCurId;
};

//*********************** BTB *********************

class BTBEntry {
public:
	bool mHistory;
	UINT32 mTag;
	UINT32 mTargetPC;
	UINT32 mLRU;
	bool mValid;
	BTBEntry() : mHistory(false), mTag(0),mTargetPC(0), mLRU(0), mValid(false){}
};

class BTB {
private:
	BTBEntry mEntries[BTBSIZE];
	UINT32 getLRUEntry();
	void updateLRUEntry(UINT32 i);
public:
	BTB();
	UINT32 getNextInstPC(UINT32 pc);
	bool addBranchBTB(UINT32 pc);
	void updateBTBEntry(UINT32 pc, bool taken, INT32 offset);
	// Todo update BTB on execution
	bool getHistory(UINT32 pc);
	void setHistory(UINT32 pc, bool history);
	void setTargetPC(UINT32 pc, UINT32 targetPC);
	UINT32 getTargetPC(UINT32 pc);
	void printBTB(std::ofstream* ofile);
};

#endif /* BUFFERS_H_ */
