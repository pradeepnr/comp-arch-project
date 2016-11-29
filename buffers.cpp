/*
 * reservationStation.cpp
 *
 *  Created on: 17-Nov-2016
 *      Author: pradeepnr
 */

#include "buffers.h"
#include <iostream>

bool ReservationStation::isRSSlotFree() {
	return (mList.size() < RSSIZE);
}

RSEntry* ReservationStation::getFreeSlot() {
	RSEntry* entry = new RSEntry();
	entry->mReady[0] = true;
	entry->mReady[1] = true;
	entry->mRobId = 0;
	entry->mExecuted = false;
	entry->mId = mCurId;
	mCurId = (mCurId+1) % RSSIZE;
	mList.push_back(entry);
	return entry;
}

void ReservationStation::releaseEntry(UINT32 rsID)
{
	LRSE::iterator it = mList.begin();
	while(it != mList.end() && (*it)->mId!=rsID) it++;
	if(it != mList.end() && (*it)!=NULL && (*it)->mId == rsID) {
		RSEntry* entry = static_cast<RSEntry*>(*it);
		mList.erase(it);
		delete (entry);

	}
}

void ReservationStation::printRS(std::ofstream* ofile) {
	LRSE::iterator it = mList.begin();
	LRSE::iterator end = mList.end();
	(*ofile)<<"RS:\n";
	while(it != end) {
#if LOG
		std::cout<<(*it)->mInst->getPrintStr()<<" Param[0]="<<(*it)->mParam[0]<<" mReady[0]="<<(*it)->mReady[0];
		std::cout<<" Param[1]="<<(*it)->mParam[1]<<" mReady[1]="<<(*it)->mReady[1]<<" Executed bit->"<<(*it)->mExecuted<<std::endl;
#else
		(*ofile)<<"["<<(*it)->mInst->getPrintStr()<<"]"<<std::endl;
#endif
		++it;
	}
}

void ReservationStation::reset() {
	while(!mList.empty()) {
		RSEntry* entry = mList.front();
		delete entry;
		mList.pop_front();
	}
}


//********* ROB methods ********************************************

bool ROB::isROBSlotFree() {
	return (mList.size() < ROBSIZE);
}

ROBEntry* ROB::getCommit() {
	if(!mList.empty() && mList.front()->mDone) {
		//std::cout<<"ROB size->"<<mList.size()<<std::endl;
		ROBEntry* first = mList.front();
		first->mComitted = true;
		return first;
	}
	return NULL;
}
void ROB::popLastCommit() {
	if(!mList.empty())
		mList.pop_front();
}

bool ROB::isNoEarlyStoreWithAddr(UINT32 robid, UINT32 addr) {
	LROBE::iterator it = mList.begin();
	while((*it)->mId!= robid) {
		if((*it)->mInst->type() == MIPS::OPSW) {
			// there should be no !mDone Store, if it is there it is ready
			// for commit then it's addre should be different
			if(!(*it)->mDone || (*it)->mStoreaddr == addr) {
				return false;
			}
		}
		it++;
	}
	return true;
}

ROBEntry* ROB::getFreeSlot() {
	ROBEntry* entry = new ROBEntry();
	entry->mDestReg = -1; // not define
	entry->mDone = false; // not ready to commit
	entry->mComitted = false; // comtted, entry will be removed in next cycle
	entry->mStoreaddr = UINT_MAX;
	entry->mId = mCurId;
	mCurId = (mCurId+1) % ROBSIZE;
	entry->mValue = -1; // not calculated yet
	mList.push_back(entry);
	return entry;
}

ROBEntry* ROB::getEntry(UINT32 id) {
	LROBE::iterator it = mList.begin();
	LROBE::iterator end = mList.end();
	while(it!=end) {
		if((*it)->mId == id) {
			return (*it);
		}
			it++;
	}
	return NULL;
}

bool ROB::hasNOP() {
	LROBE::iterator it = mList.begin();
	LROBE::iterator end = mList.end();
	while(it!=end) {
		if((*it)->mInst->type() == MIPS::OPNOP) {
			return true;
		}
		it++;
	}
	return false;
}

void ROB::printRob(std::ofstream* ofile) {
	LROBE::iterator it = mList.begin();
	LROBE::iterator end = mList.end();
	(*ofile)<<"ROB:\n";
	while(it!=end) {
		if(!(*it)->mComitted) {
#if LOG
			std::cout<<(*it)->mInst->getPrintStr()<<" Robid->"<<(*it)->mId<<" DestiId="<<(*it)->mDestReg<<" value="<<(*it)->mValue<<" Done="<<(*it)->mDone<<"\n";
#else
			(*ofile)<<"["<<(*it)->mInst->getPrintStr()<<"]"<<std::endl;
#endif
		}
		++it;
	}
}

void ROB::reset() {
	// Clear the list completely and just add passed entry to it
	while(!mList.empty()) {
		ROBEntry* entry = mList.front();
		delete entry->mInst;
		delete entry;
		mList.pop_front();
	}
	//mList.push_back(robEntry);
}


//****************** BTB ***********************

BTB::BTB() {
	for(UINT32 i=0; i<BTBSIZE; ++i) {
		mEntries[i].mLRU = i;
	}
}

UINT32 BTB::getLRUEntry() {
	UINT32 id=0;
	for(UINT32 i=0; i<BTBSIZE; ++i) {
		if(0 == mEntries[i].mLRU) {
			id = i;
		}
		else {
			mEntries[i].mLRU -=1;
		}
	}
	mEntries[id].mLRU = BTBSIZE-1;
	return id;
}

void BTB::updateLRUEntry(UINT32 id) {
	UINT32 lru = mEntries[id].mLRU;
	for(INT32 i=0; i<BTBSIZE; ++i) {
		if(mEntries[i].mLRU > lru) {
			mEntries[i].mLRU -= 1;
		}
	}
	mEntries[id].mLRU = BTBSIZE-1;
}

UINT32 BTB::getNextInstPC(UINT32 pc) {
	//TODO implement LRU
	int i = 0;
	for(;i<BTBSIZE;++i) {
		if(mEntries[i].mTag == pc && mEntries[i].mValid) {
			break;
		}
	}
	if(i<BTBSIZE && mEntries[i].mHistory && mEntries[i].mValid) {
		updateLRUEntry(i);
		return mEntries[i].mTargetPC;
	}
	return 0;
}

bool BTB::addBranchBTB(UINT32 pc) {
	for(int i=0;i<BTBSIZE;++i) {
		if(mEntries[i].mTag == pc)
			return false;
	}
	UINT32 id = getLRUEntry();
	mEntries[id].mHistory = false;
	mEntries[id].mTag = pc;
	mEntries[id].mTargetPC = pc+4;
	mEntries[id].mValid = true;

	return true;
}

void BTB::updateBTBEntry(UINT32 pc, bool taken, INT32 offset) {
	int i=0;
	while(i<BTBSIZE && mEntries[i].mTag!=pc) {
		++i;
	}
	updateLRUEntry(i);
	mEntries[i].mHistory = taken;
	mEntries[i].mTargetPC = mEntries[i].mTag + 4 + offset;
}

bool BTB::getHistory(UINT32 pc) {
	int i=0;
	while(i<BTBSIZE && mEntries[i].mTag!=pc) {
		++i;
	}
	updateLRUEntry(i);
	return mEntries[i].mHistory;
}

void BTB::setTargetPC(UINT32 pc, UINT32 targetPC) {
	int i=0;
	while(i<BTBSIZE && mEntries[i].mTag!=pc) {
		++i;
	}
	updateLRUEntry(i);
	mEntries[i].mTargetPC = targetPC;
}

UINT32 BTB::getTargetPC(UINT32 pc) {
	int i=0;
	while(i<BTBSIZE && mEntries[i].mTag!=pc) {
		++i;
	}
	updateLRUEntry(i);
	return mEntries[i].mTargetPC;
}

void BTB::setHistory(UINT32 pc, bool history) {
	int i=0;
	while(i<BTBSIZE && mEntries[i].mTag!=pc) {
		++i;
	}
	updateLRUEntry(i);
	mEntries[i].mHistory = history;
}

void BTB::printBTB(std::ofstream* ofile) {
	(*ofile)<<"BTB:\n";
	for(int i=0;i<BTBSIZE;++i) {
		if(mEntries[i].mValid) {
			(*ofile)<<"[Entry "<<i+1<<"]<"<<mEntries[i].mTag<<","<<mEntries[i].mTargetPC<<",";
			if(mEntries[i].mHistory == true) {
				(*ofile)<<"1>\n";
			}
			else {
				(*ofile)<<"0>\n";
			}
		}
	}
}
