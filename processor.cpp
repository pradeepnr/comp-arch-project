/*
 * processor.cpp
 *
 *  Created on: 16-Nov-2016
 *      Author: pradeepnr
 */
// g++ -std=gnu++11 mipsSimulator.cpp mips.cpp btb.cpp instructionSet.cpp processor.cpp utility.cpp -o MIPSsim

#include "processor.h"
#include "utility.h"
#include <vector>
#include <iostream>

Processor::Processor(std::string ipfilename, std::string opfilename, INT32 m, INT32 n)
	: mPC(BASEADDR),
	  mIPFilename(ipfilename),
	  mOPFilename(opfilename),
	  mIFile(NULL),
	  mOFile(NULL),
	  mRun(true),
	  mPopRob(false),
	  mMissPredicted(false),
	  mNewPC(0),
	  mStopFetch(false),
	  mStartC(m),
	  mEndC(n) {
	mIFile = new std::ifstream(mIPFilename, std::ifstream::binary);
	mOFile = new std::ofstream(mOPFilename, std::ofstream::binary);
	for(INT32 i=0;i<32;++i) {
		mReg[i] = 0;
		mRAT[i] = -1;
	}
}

void Processor::init() {
	UINT32 val = 0;
	UINT32 buf;
	UINT32 pc = BASEADDR;
	while(mIFile->read((char*)&buf,4) && !mIFile->eof()) {
		val = buf;
		if(Utility::isLittleEndian()) {
			val = Utility::swapUINT32(buf);
		}
		mMem[pc] = val;
		pc+=4;
	}
}
Processor::~Processor() {
	if(mIFile)
		delete mIFile;
	if(mOFile)
		delete mOFile;
}

void Processor::process() {
	INT32 i = 1;

	Dispay display = NONE;
	//std::cout<<mStartC<<" "<<mEndC<<std::endl;
	if(-1 == mStartC && -1 == mEndC) {
		display = COMPLETE;
	}
	else if(0 == mStartC && 0 == mEndC) {
		display = FINAL;
	}
	else if(mStartC >0 && mEndC > 0 && mStartC <= mEndC) {
		display = PARTIAL;
	}
//for(int jj =0;jj<51;++jj) {
	while(mRun) {
		commit();
		writeResult();
		memory();
		execute();
		decode();
		fetch();

		if(display == COMPLETE || ( display == PARTIAL && (i>=mStartC && i<=mEndC))) {
			(*mOFile)<<"Cycle <"<<i<<">:\n";
			DisplayState();
		}

		if(mMissPredicted) {
			mPC = mNewPC;
			mMissPredicted = false;
		}
		i++;
		//std::cout<<"\n\n";
	}
	if(display == FINAL) {
		DisplayState();
	}
	mOFile->flush();
	std::cout<<"Output is present at "<<mOPFilename<<std::endl;
}

void Processor::DisplayState() {
	printIQ();
	//printRAT();
	//printREG();
	mRS.printRS(mOFile);
	mRob.printRob(mOFile);
	mBTB.printBTB(mOFile);
	printRegs();
	printMem();
	//std::cout<<"\n\n";
}

void Processor::fetch() {
	if(mPC >= ENDADDR || mStopFetch) {
#if LOG
		std::cout<<"Stop Fetching\n";
#endif
		return;
	}
	//std::cout<<"fetch PC->"<<mPC<<"\n";
	MIPS::Instruction* inst = InstructionSet::decode(mMem[mPC], mPC);
#if LOG
	std::cout<<"Fetch -------------->"<<inst->getPrintStr()<<std::endl;
#endif
	//std::cout<<"instruction fetch->"<<inst->getPrintStr()<<std::endl;
	if(MIPS::OPBREAK == inst->type()) {
		mStopFetch = true;
	}
	mInstQueue.push(inst);
	UINT32 pc = mBTB.getNextInstPC(mPC);
	if(pc == 0) {
	 mPC += 4;
	} else {
	 mPC = pc;
	}
}

void Processor::decode() {
	if(mInstQueue.empty()) {
		//std::cout<<"Instruction Queue empty\n";
		return;
	}

	MIPS::Instruction* inst = mInstQueue.front();
#if LOG
	std::cout<<"Decode -------------->"<<inst->getPrintStr()<<std::endl;
#endif
	if( (MIPS::OPBREAK == inst->type() || MIPS::OPNOP == inst->type()) && mRob.isROBSlotFree()) {
		mInstQueue.pop();
		if((MIPS::OPNOP == inst->type() && !mRob.hasNOP()) || MIPS::OPBREAK == inst->type()) {
			ROBEntry* robentry = mRob.getFreeSlot();
			robentry->mInst = inst;
			robentry->mDone = true;
		}
	}
	else if(mRS.isRSSlotFree() && mRob.isROBSlotFree()) {
		//std::cout<<"Decode\n";
		mInstQueue.pop();
		// ROB entry
		ROBEntry* robentry = mRob.getFreeSlot();
		robentry->mInst = inst;
		robentry->mDestReg = inst->getDRegId(); // invalid in case of SW
		// RS entry
		RSEntry* rsentry = mRS.getFreeSlot();
		rsentry->mRobId = robentry->mId;
		rsentry->mInst = inst;

		// Required to release entry during commit
		robentry->mRSid = rsentry->mId;

		// We won't write any thing to register in case of store but only to memory
		if(InstructionSet::IsStore(inst)) {
			MIPS::SW* sw = static_cast<MIPS::SW*>(inst);
			// param 0 -> base reg value, not index
			// param 1 -> regT reg value, not index
			UINT32 reg = 0;
			for(int si=0;si<2;++si) {
				if(si == 0) {
					reg = sw->getBase();
				}
				else {
					reg = sw->getTReg();
				}
				if(mRAT[reg] == -1) {
					rsentry->mParam[si] = mReg[reg];
					rsentry->mReady[si] = true;
				}
				else {
					ROBEntry* robentry = mRob.getEntry(mRAT[reg]);
					if(robentry->mDone) {
						rsentry->mReady[si] = true;
						rsentry->mParam[si] = robentry->mValue;
					}
					else {
						rsentry->mReady[si] = false;
						rsentry->mParam[si] = robentry->mId;
					}
				}
			}
		}
		else if(InstructionSet::IsBranch(inst)) {
			UINT32 rc = inst->regCount();
			for(int bi=0;bi<rc;++bi) {
				UINT32 reg = inst->getSRegId(bi);
#if LOG
				std::cout<<"Decode->"<<inst->getPrintStr()<<" reg="<<reg<<" mRAT[reg]="<<mRAT[reg]<<" robentry->Done="<<robentry->mDone<<" robentry->mId="<<robentry->mId<<std::endl;
#endif
				if(mRAT[reg] == -1) {
					rsentry->mParam[bi] = mReg[reg];
					rsentry->mReady[bi] = true;
				}
				else {
					ROBEntry* robentry = mRob.getEntry(mRAT[reg]);
					if(robentry->mDone) {
						rsentry->mReady[bi] = true;
						rsentry->mParam[bi] = robentry->mValue;
					}
					else {
						rsentry->mReady[bi] = false;
						rsentry->mParam[bi] = robentry->mId;
					}
				}
			}
		}
		else {
			UINT32 rc = inst->regCount();
			for(UINT32 i=0;i<rc;++i) {
				UINT32 Regid = inst->getSRegId(i);
				if(mRAT[Regid] == -1) { // Reg is fine read from it
					rsentry->mReady[i] = true;
					rsentry->mParam[i] = mReg[Regid];
				}
				else { // use rob entry
					ROBEntry* robentry = mRob.getEntry(mRAT[Regid]);
					if(robentry->mDone) { // expression is evaluated so take value
						rsentry->mReady[i] = true;
						rsentry->mParam[i] = robentry->mValue;
					}
					else { // expression is waiting on other ROB so take its rob if
						rsentry->mReady[i] = false;
						//std::cout<<"robentry->mId="<<robentry->mId<<"\n";
						rsentry->mParam[i] = robentry->mId;//mRAT[Regid]; both should be same
					}
				}
			}
			mRAT[inst->getDRegId()] = robentry->mId;
		}
		if(rsentry->exeReady() && !rsentry->mExecuted) {
			mExeQueue.push(rsentry);
		}
	}
	else {
		//std::cout<<"Decode Stall\n";
	}
}

void Processor::execute() {

	while(!mExeQueue.empty()) {
			RSEntry* rsEntry = mExeQueue.front();
			mExeQueue.pop();
#if LOG
			std::cout<<"Execute -------------->"<<rsEntry->mInst->getPrintStr()<<std::endl;
#endif
			rsEntry->mExecuted = true;
			if(InstructionSet::IsBranch(rsEntry->mInst)) {
				if(rsEntry->mInst->type() == MIPS::OPJ) {
					MIPS::J* j = static_cast<MIPS::J*>(rsEntry->mInst);
					ROBEntry* robentry = mRob.getEntry(rsEntry->mRobId);
					robentry->mDone = true;
					robentry->mValue = j->getTarget();
				}
				else {
					// If branch is not present in btb create new entry
					bool added = mBTB.addBranchBTB(rsEntry->mInst->getPC());
					if(added) {
						INT32 branchOffset = getBranchOffset(rsEntry);
						mBTB.updateBTBEntry(rsEntry->mInst->getPC(), false, branchOffset);
					}
					bool branchTaken = isBranchTaken(rsEntry);
					//std::cout<<"EXECUTED->"<<(*itRS)->mInst->getPrintStr()<<" branchTaken is "<<branchTaken<<std::endl;
					bool history = mBTB.getHistory(rsEntry->mInst->getPC());
					mBTB.setHistory(rsEntry->mInst->getPC(), branchTaken);
					ROBEntry* robentry = mRob.getEntry(rsEntry->mRobId);
					robentry->mDone = true;
					// This will tell commit() wheather to reset buffer or not
					// mValue == false everything is fine; mValue == true wrong inst fetched so clear buffer
					robentry->mValue = (branchTaken != history);
					// elseIf present and mispredicted update entry
				}
			}
			else if(InstructionSet::IsLoad(rsEntry->mInst)) {
				// Execute inorder
				// when basic register is ready, check if not early store with same addr in ROB
				MIPS::LW* lw = static_cast<MIPS::LW*>(rsEntry->mInst);
				UINT32 srcAddr = rsEntry->mParam[0] + lw->getOffset();
				if(mRob.isNoEarlyStoreWithAddr(rsEntry->mRobId, srcAddr )) {
					ROBEntry* robentry = mRob.getEntry(rsEntry->mRobId);
					//Postpone making robentry Done till memory stage
					//robentry->mDone = true;
					lw->mCycle = 1;
					robentry->mValue = srcAddr;
					robentry->mDestReg = lw->getDRegId();
					mMemQueue.push(rsEntry);
				}
			}
			else if (InstructionSet::IsStore(rsEntry->mInst)) {
				MIPS::SW* sw = static_cast<MIPS::SW*>(rsEntry->mInst);
				//std::cout<<sw->getPrintStr()<<std::endl;
				UINT32 destAddr = rsEntry->mParam[0] + sw->getOffset();
				if(mRob.isNoEarlyStoreWithAddr(rsEntry->mRobId, destAddr )) {
					ROBEntry* robentry = mRob.getEntry(rsEntry->mRobId);
					robentry->mDone = true;
					robentry->mStoreaddr = destAddr;
					robentry->mValue = rsEntry->mParam[1];
					//sw->mCycle = 1;
				}
			} else {
				// delete the RS entry at end
				INT32 res = exeInst(rsEntry);
				//std::cout<<"Executed "<<(*itRS)->mInst->getPrintStr()<<" res="<<res<<std::endl;
				PKT packet = std::make_pair(rsEntry->mRobId, res);
				mCDB.push(packet);
			}
		}

	LRSE::iterator itRS = mRS.getRSItrBegin();
	LRSE::iterator end = mRS.getRSItrEnd();
	//collection of all RS entries to be deleted at end
	while(itRS != end) {
		if((*itRS)->exeReady() && !(*itRS)->mExecuted) {
				mExeQueue.push((*itRS));
		}
		itRS++;
	}
	//delete all the executed RS entries

}

void Processor::memory() {
		while(!mMemQueue.empty()) {
			RSEntry* rsentry = mMemQueue.front();
			mMemQueue.pop();
			if(InstructionSet::IsLoad(rsentry->mInst)) {
				MIPS::LW* lw = static_cast<MIPS::LW*>(rsentry->mInst);
#if LOG
				std::cout<<"Memory -------------->"<<lw->getPrintStr()<<std::endl;
#endif
				ROBEntry* robentry = mRob.getEntry(rsentry->mRobId);
				robentry->mValue = mMem[robentry->mValue];
				//robentry->mDone = true;
				PKT packet = std::make_pair(robentry->mId, robentry->mValue);
				mCDB.push(packet);
			}
		}
}

//Broadcast
void Processor::writeResult() {
	while(!mCDB.empty()) {
		//update RS
		PKT pkt = mCDB.front();
		mCDB.pop();
		updateRS(pkt);
		//update ROB with value
		ROBEntry* robentry = mRob.getEntry(pkt.first);
#if LOG
		std::cout<<"Write Result -------------->"<<robentry->mInst->getPrintStr()<<std::endl;
#endif
		robentry->mDone = true;
		robentry->mValue = pkt.second;
	}
}

void Processor::updateRS(PKT& pkt) {
	LRSE::iterator itRS = mRS.getRSItrBegin();
	LRSE::iterator itRSEnd = mRS.getRSItrEnd();
	while(itRS != itRSEnd) {
		//std::cout<<"writeResult\n";
		RSEntry* rsentry = (*itRS);

		// update RS
		if(!rsentry->exeReady()) {
			INT32 rc = 0;
			if(InstructionSet::IsStore((*itRS)->mInst)) {
				rc = 2;
			}
			else {
				rc = rsentry->mInst->regCount();
			}
			for(int i=0;i<rc;i++) {
				if(!rsentry->mReady[i] && rsentry->mParam[i] == pkt.first) {
					rsentry->mReady[i] = true;
					rsentry->mParam[i] = pkt.second;//value
				}
			}
		}

		itRS++;
	}
}

void Processor::commit() {
	// RS clearn up
	//std::vector<LRSE::iterator> vec;
	if(mPopRob) {
		mRob.popLastCommit();
		mPopRob = false;
	}
	ROBEntry* robEntry = mRob.getCommit();
	if(!robEntry) {
#if LOG
		std::cout<<"Nothing to commit\n";
#endif
		return;
	}
	mPopRob = true;
#if LOG
	std::cout<<"Commit --------------->"<<robEntry->mInst->getPrintStr()<<std::endl;
#endif
	if(InstructionSet::IsBranch(robEntry->mInst)) {
		if(robEntry->mInst->type() == MIPS::OPJ) {
			bool added = mBTB.addBranchBTB(robEntry->mInst->getPC());
			// For Jump only once set target and reset buffer
			// Target doesn't change for it
			if(added) {
				MIPS::J* j = static_cast<MIPS::J*>(robEntry->mInst);
				mBTB.setTargetPC(robEntry->mInst->getPC(), j->getTarget());
				mBTB.setHistory(robEntry->mInst->getPC(), true);
				//mPC = j->getTarget();

				// new pc value will be updated at the end of the cycle in process()
				mPC = ENDADDR;
				mNewPC = j->getTarget();
				mMissPredicted = true;
				// reset ROB and RS and make RAT set to -1
				resetBuffers();
			}
		}
		else {
			bool clearBuffer = robEntry->mValue == 1 ? true : false;
			if(clearBuffer) {
				// reset ROB and RS and make RAT set to -1
#if LOG
				std::cout<<"Commit ->clearning buffer\n";
#endif
				//mPC = mBTB.getTargetPC(robEntry->mInst->getPC());

				// new pc value will be updated at the end of the cycle in process()
				mPC = ENDADDR;
				mNewPC = mBTB.getTargetPC(robEntry->mInst->getPC());
				mMissPredicted = true;
				resetBuffers();
			}
		}
	}
	else if(InstructionSet::IsStore(robEntry->mInst)) {
#if LOG
		MIPS::SW* sw = static_cast<MIPS::SW*>(robEntry->mInst);
		std::cout<<"IsStore mCycle->"<<sw->mCycle<<" mMem["<<robEntry->mStoreaddr<<"]="<<robEntry->mValue<<std::endl;
#endif
		//if(sw->mCycle == 2) {
			mMem[robEntry->mStoreaddr] = robEntry->mValue;
			//sw->mCycle = 0;
		//}
	}
	else if (InstructionSet::IsLoad(robEntry->mInst)) {
		MIPS::LW* lw = static_cast<MIPS::LW*>(robEntry->mInst);
		//std::cout<<"Commit LW mCycle->"<<lw->mCycle<<std::endl;
		//if(lw->mCycle == 2) {
			//std::cout<<"Comitting->mReg["<<robEntry->mDestReg<<"]="<<robEntry->mValue<<std::endl;
			//std::cout<<"Comitting mRAT as -1\n";
			// update RS about previous Load operation
			mReg[robEntry->mDestReg] = robEntry->mValue; // memory is accessed in one intermediate step memory()
			mRAT[robEntry->mDestReg] = -1;
			lw->mCycle = 0;
			PKT pkt = std::make_pair(robEntry->mId, robEntry->mValue);
			updateRS(pkt);
		//}
	}
	else if(robEntry->mInst->type() == MIPS::OPNOP) {

	}
	else if(robEntry->mInst->type() == MIPS::OPBREAK) {
#if LOG
			std::cout<<"STOPPPPPPPPPPPPPPPPPPP\n";
#endif
			mRun = false;
	} else {
		//if commit head is pointing to normal instruction
		// update Reg, RAT and memory
		mReg[robEntry->mDestReg] = robEntry->mValue;
		mRAT[robEntry->mDestReg] = -1; // Reg is good

		mMem[robEntry->mDestReg] = robEntry->mValue;
	}

	mRS.releaseEntry(robEntry->mRSid);
}

INT32 Processor::getBranchOffset(RSEntry* ent) {
	INT32 offset = 0;
	switch(ent->mInst->type()) {
		case MIPS::OPBEQ:
		{
			MIPS::BEQ* beq = static_cast<MIPS::BEQ*>(ent->mInst);
			offset = beq->getOffset();
		}
		break;
		case MIPS::OPBNE:
		{
			MIPS::BNE* bne = static_cast<MIPS::BNE*>(ent->mInst);
			offset = bne->getOffset();
		}
		break;
		case MIPS::OPBGEZ:
		{
			MIPS::BGEZ* bgez = static_cast<MIPS::BGEZ*>(ent->mInst);
			offset = bgez->getOffset();
		}
		break;
		case MIPS::OPBGTZ:
		{
			MIPS::BGTZ* bgtz = static_cast<MIPS::BGTZ*>(ent->mInst);
			offset = bgtz->getOffset();
		}
		break;
		case MIPS::OPBLEZ:
		{
			MIPS::BLEZ* blez = static_cast<MIPS::BLEZ*>(ent->mInst);
			offset = blez->getOffset();
		}
		break;
		case MIPS::OPBLTZ:
		{
			MIPS::BLTZ* bltz = static_cast<MIPS::BLTZ*>(ent->mInst);
			offset = bltz->getOffset();
		}
		break;
		default:
		{

		}
	}
	return offset;
}

bool Processor::isBranchTaken(RSEntry* ent) {
	bool taken = false;
	switch(ent->mInst->type()) {
		case MIPS::OPBEQ:
		{
			if(ent->mParam[0] == ent->mParam[1]) {
				taken = true;
			}
		}
		break;
		case MIPS::OPBNE:
		{
			if(ent->mParam[0] != ent->mParam[1]) {
				taken = true;
			}
		}
		break;
		case MIPS::OPBGEZ:
		{
			INT32 temp= static_cast<INT32>(ent->mParam[0]);
			if(temp >=0) {
				taken = true;
			}
		}
		break;
		case MIPS::OPBGTZ:
		{
			if(ent->mParam[0]>0) {
				taken = true;
			}
		}
		break;
		case MIPS::OPBLEZ:
		{
			if(ent->mParam[0]<=0) {
				taken = true;
			}
		}
		break;
		case MIPS::OPBLTZ:
		{
			INT32 temp= static_cast<INT32>(ent->mParam[0]);
			if(temp<0) {
				taken = true;
			}
		}
		break;
		default:
		{

		}
	}
	return taken;
}

void Processor::resetBuffers() {
	mRob.reset();
	mRS.reset();
	for(int i=0;i<32;++i) {
		mRAT[i] = -1;
	}
	while(!mInstQueue.empty()) {
		delete mInstQueue.front();
		mInstQueue.pop();
	}
	while(!mCDB.empty()){
		mCDB.pop();
	}
	while(!mExeQueue.empty()) {
		mExeQueue.pop();
	}
	while(!mMemQueue.empty()) {
		mMemQueue.pop();
	}
	mStopFetch = false;
}

INT32 Processor::exeInst(RSEntry* ent) {
	INT32 res = INT_MIN;
	switch(ent->mInst->type()) {
		case MIPS::OPADDI:
		{
			MIPS::ADDI* addIns = static_cast<MIPS::ADDI*> (ent->mInst);
			res = ent->mParam[0] + addIns->getImm();
			break;
		}
		case MIPS::OPADD:
		{
			res = ent->mParam[0] + ent->mParam[1];
			break;
		}
		case MIPS::OPADDU:
		{
			res = ent->mParam[0] + ent->mParam[1];
			break;
		}
		case MIPS::OPADDIU:
		{
			MIPS::ADDIU* addiu = static_cast<MIPS::ADDIU*>(ent->mInst);
			res = ent->mParam[0] + addiu->getImm();
			break;
		}
		case MIPS::OPSLT:
		{
			res = (ent->mParam[0] < ent->mParam[1]);
			break;
		}
		case MIPS::OPSLTI:
		{
			MIPS::SLTI* slti = static_cast<MIPS::SLTI*>(ent->mInst);
			res = (ent->mParam[0] < slti->getImm());
			break;
		}
		case MIPS::OPSLTU:
		{
			res = (ent->mParam[0] < ent->mParam[1]);
			break;
		}
		case MIPS::OPSLL:
		{
			MIPS::SLL* sll = static_cast<MIPS::SLL*>(ent->mInst);
			res = (ent->mParam[0] << sll->getSA());
			break;
		}
		case MIPS::OPSRL:
		{
			MIPS::SRL* srl = static_cast<MIPS::SRL*>(ent->mInst);
			res = (ent->mParam[0] >> srl->getSA());
			break;
		}
		case MIPS::OPSRA:
		{
			MIPS::SRA* sra = static_cast<MIPS::SRA*>(ent->mInst);
			res = ent->mParam[0] >> sra->getSA();
			break;
		}
		case MIPS::OPSUB:
		{
			res = ent->mParam[0] - ent->mParam[1];
			break;
		}
		case MIPS::OPSUBU:
		{
			res = ent->mParam[0] - ent->mParam[1];
			break;
		}
		case MIPS::OPAND:
		{
			res = (ent->mParam[0] & ent->mParam[1]);
			break;
		}
		case MIPS::OPOR:
		{
			res = (ent->mParam[0] | ent->mParam[1]);
			break;
		}
		case MIPS::OPXOR:
		{
			res = ent->mParam[0] ^ ent->mParam[1];
			break;
		}
		case MIPS::OPNOR:
		{
			res = !(ent->mParam[0] | ent->mParam[1]);
			break;
		}
		default:
		{

		}
	}
	return res;
}

void Processor::printRegs() {
	(*mOFile)<<"Registers:\n";
	for(int i=0;i<4;++i) {
		if(i*8<10)
			(*mOFile)<<"R0"<<i*8<<":";
		else
			(*mOFile)<<"R"<<i*8<<":";
		for(int j=0;j<8;++j) {
			(*mOFile)<<"	"<<mReg[i*8+j];
		}
		(*mOFile)<<"\n";
	}
}

void Processor::printMem() {
	(*mOFile)<<"Data Segment:\n";
	(*mOFile)<<BASEADDR+116<<":";
	for(int i=0;i<10;++i) {
		//std::cout<<"	"<<"M["<<BASEADDR+116+i*4<<"]="<<mMem[BASEADDR+116+i*4];
		(*mOFile)<<"	"<<mMem[BASEADDR+116+i*4];
	}
	(*mOFile)<<"\n";
}

void Processor::printIQ() {
	UINT32 size = mInstQueue.size();
	(*mOFile)<<"IQ:\n";
	for(int i=0;i<size;++i) {
		MIPS::Instruction* inst = mInstQueue.front();
		mInstQueue.pop();
		(*mOFile)<<"["<<inst->getPrintStr()<<"]\n";
		mInstQueue.push(inst);
	}
}

void Processor::printRAT() {
	std::cout<<"RAT:\n";
	for(int i=0;i<32;++i) {
		std::cout<<" ["<<i<<"]="<<mRAT[i];
	}
	std::cout<<"\n";
}
void Processor::printREG() {
	std::cout<<"REG:\n";
	for(int i=0;i<32;++i) {
		std::cout<<" ["<<i<<"]="<<mReg[i];
	}
	std::cout<<"\n";
}
