#ifndef MIPS_H_
#define MIPS_H_
#include "types.h"
#include <string>
#include <bitset>
#include <climits>

namespace MIPS {

enum OpCode {
	OPSW, OPLW, OPJ, OPBEQ, OPBNE, OPBGEZ, OPBGTZ, OPBLEZ, OPBLTZ, OPADDI, OPADDIU, OPBREAK, OPSLT,
	OPSLTI, OPSLTU, OPSLL, OPSRL, OPSRA, OPSUB, OPSUBU, OPADD, OPADDU, OPAND, OPOR, OPXOR, OPNOR, OPNOP
};

class Instruction {
public:
	Instruction(UINT32 val, INT32 pc) : mCode(val), mPC(pc) {}
	virtual ~Instruction() {}
	virtual std::string& getPrintStr() = 0;
	virtual OpCode type() = 0;
	UINT32 getInstruction() { return mCode; }
	virtual UINT32 regCount() { return 0; } // returns number of source registers in the instruction
	virtual UINT32 getSRegId(UINT32 id) {return UINT_MAX;};
	virtual UINT32 getDRegId() {return UINT_MAX;};
	UINT32 getPC() { return mPC; }
protected:
	UINT32 mCode;
	INT32 mPC;
};

class ADDI : public Instruction {
public:
	ADDI(UINT32 val, INT32 pc);
	std::string& getPrintStr() { return printStr;}
	UINT32 regCount() { return rno; }
	UINT32 getSRegId(UINT32 id)
	{
		if(id<rno) {
			return regS;
		}
		return UINT_MAX;
	};
	UINT32 getDRegId() {return regT;}
	INT16 getImm() { return imm; }
	OpCode type() { return OPADDI;}
private:
	static const UINT32 rno = 1;
	UINT32 regT;
	UINT32 regS;
	INT16 imm;
	std::string printStr;
};

// it is signed so consider last bit as signed
class ADD : public Instruction {
public:
	ADD(UINT32 val, INT32 pc);
	std::string& getPrintStr() { return printStr;}
	OpCode type() { return OPADD;}
	UINT32 regCount() { return rno; }
	UINT32 getSRegId(UINT32 id)
		{
			if(id==0) {	return regS; }
			else if(id == 1) {return regT;}
			else return UINT_MAX;
		};
	UINT32 getDRegId() {return regD;}
private:
	static const UINT32 rno = 2;
	UINT32 regD;
	UINT32 regS;
	UINT32 regT;
	std::string printStr;
};

// it is unsigned so consider all elements as number
class ADDU : public Instruction {
public:
	ADDU(UINT32 val, INT32 pc);
	std::string& getPrintStr() { return printStr;}
	OpCode type() { return OPADDU;}
	UINT32 regCount() { return rno; }
	UINT32 getSRegId(UINT32 id)
	{
		if(id==0) {
			return regS;
		}
		else if(id==1) {
			return regT;
		}
		return UINT_MAX;
	};
	UINT32 getDRegId() {return regD;}

private:
	static const UINT32 rno = 2;
	UINT32 regD;
	UINT32 regS;
	UINT32 regT;
	std::string printStr;
};

class SW : public Instruction {
public:
	SW(UINT32 val, INT32 pc);
	std::string& getPrintStr() { return printStr;}
	UINT32 regCount() { return rno; }
	OpCode type() { return OPSW;}
	UINT32 getOffset() { return offset; }
	UINT32 getBase() { return base; }
	UINT32 getTReg() { return regT; }
	UINT16 mCycle;
private:
	static const UINT32 rno = 0;
	UINT32 base;
	UINT32 regT;
	INT16 offset;
	std::string printStr;
};

class LW : public Instruction {
public:
	LW(UINT32 val, INT32 pc);
	std::string& getPrintStr() { return printStr;}
	UINT32 regCount() { return rno; }
	UINT32 getSRegId(UINT32 id)
	{
		if(id == 0) {
			return base;
		}
		return UINT_MAX;
	};
	UINT32 getDRegId() {return regT;}
	OpCode type() { return OPLW;}
	UINT32 getOffset() { return offset; }
	UINT32 getBase() { return base; }
	UINT16 mCycle;
private:
	static const UINT32 rno = 1;
	UINT32 base;
	UINT32 regT;
	INT16 offset;
	std::string printStr;
};

class J : public Instruction {
public:
	J(UINT32 val, INT32 pc);
	std::string& getPrintStr() { return printStr;}
	OpCode type() { return OPJ;}
	UINT32 getTarget() { return target; }
	UINT32 regCount() { return rno; }
private:
	static const UINT32 rno = 0;
	UINT32 target:28;
	std::string printStr;
};

class BEQ : public Instruction {
public:
	BEQ(UINT32 val, INT32 pc);
	std::string& getPrintStr() { return printStr;}
	OpCode type() { return OPBEQ;}
	UINT32 regCount() { return rno; }
	UINT32 getSRegId(UINT32 id)
	{
		if(id == 0) {
			return regS;
		}
		else if(id == 1) {
			return regT;
		}
		return UINT_MAX;
	};
	INT32 getOffset() { return offset; }
private:
	static const UINT32 rno = 2;
	UINT32 regS;
	UINT32 regT;
	INT32 offset:18;
	std::string printStr;
};

class BNE : public Instruction {
public:
	BNE(UINT32 val, INT32 pc);
	std::string& getPrintStr() { return printStr;}
	OpCode type() { return OPBNE;}
	UINT32 regCount() { return rno; }
	UINT32 getSRegId(UINT32 id)
	{
		if(id == 0) {
			return regS;
		}
		else if(id == 1) {
			return regT;
		}
		return UINT_MAX;
	};
	INT32 getOffset() { return offset; }
private:
	static const UINT32 rno = 2;
	UINT32 regS;
	UINT32 regT;
	INT32 offset:18;
	std::string printStr;
};

class BGEZ : public Instruction {
public:
	BGEZ(UINT32 val, INT32 pc);
	std::string& getPrintStr() { return printStr;}
	OpCode type() { return OPBGEZ;}
	UINT32 regCount() { return rno; }
	UINT32 getSRegId(UINT32 id)
	{
		if(id == 0) {
			return regS;
		}
		return UINT_MAX;
	};
	INT32 getOffset() { return offset; }
private:
	static const UINT32 rno = 1;
	UINT32 regS;
	INT32 offset:18;
	std::string printStr;
};

class BGTZ : public Instruction {
public:
	BGTZ(UINT32 val, INT32 pc);
	std::string& getPrintStr() { return printStr;}
	OpCode type() { return OPBGTZ;}
	UINT32 regCount() { return rno; }
	UINT32 getSRegId(UINT32 id)
	{
		if(id == 0) {
			return regS;
		}
		return UINT_MAX;
	};
	INT32 getOffset() { return offset; }
private:
	static const UINT32 rno = 1;
	UINT32 regS;
	INT32 offset:18;
	std::string printStr;
};

class BLEZ : public Instruction {
public:
	BLEZ(UINT32 val, INT32 pc);
	std::string& getPrintStr() { return printStr;}
	OpCode type() { return OPBLEZ;}
	UINT32 regCount() { return rno; }
	UINT32 getSRegId(UINT32 id)
	{
		if(id == 0) {
			return regS;
		}
		return UINT_MAX;
	};
	INT32 getOffset() { return offset; }
private:
	static const UINT32 rno = 1;
	UINT32 regS;
	INT32 offset:18;
	std::string printStr;
};

class BLTZ : public Instruction {
public:
	BLTZ(UINT32 val, INT32 pc);
	std::string& getPrintStr() { return printStr;}
	OpCode type() { return OPBLTZ;}
	UINT32 regCount() { return rno; }
	UINT32 getSRegId(UINT32 id)
	{
		if(id == 0) {
			return regS;
		}
		return UINT_MAX;
	};
	INT32 getOffset() { return offset; }
private:
	static const UINT32 rno = 1;
	UINT32 regS;
	INT32 offset:18;
	std::string printStr;
};

class ADDIU : public Instruction {
public:
	ADDIU(UINT32 val, INT32 pc);
	std::string& getPrintStr() { return printStr;}
	OpCode type() { return OPADDIU;}
	UINT32 regCount() { return rno; }
	UINT32 getSRegId(UINT32 id)
	{
		if(id==0) {
			return regS;
		}
		return UINT_MAX;
	};
	UINT32 getDRegId() {return regT;}
	INT16 getImm() { return imm; }
private:
	static const UINT32 rno = 1;
	UINT32 regS;
	UINT32 regT;
	INT16 imm;
	std::string printStr;
};

class BREAK : public Instruction {
public:
	BREAK(UINT32 val, INT32 pc);
	std::string& getPrintStr() { return printStr;}
	OpCode type() { return OPBREAK;}
private:
	//UINT32 code;
	std::string printStr;
};

class SLT : public Instruction {
public:
	SLT(UINT32 val, INT32 pc);
	std::string& getPrintStr() { return printStr;}
	OpCode type() { return OPSLT;}
	UINT32 regCount() { return rno; }
	UINT32 getSRegId(UINT32 id)
	{
		if(id==0) {
			return regS;
		}
		else if(id==1) {
			return regT;
		}
		return UINT_MAX;
	};
	UINT32 getDRegId() {return regD;}
private:
	static const UINT32 rno = 2;
	UINT32 regS;
	UINT32 regT;
	UINT32 regD;
	std::string printStr;
};

class SLTI : public Instruction {
public:
	SLTI(UINT32 val, INT32 pc);
	std::string& getPrintStr() { return printStr;}
	OpCode type() { return OPSLTI;}
	UINT32 regCount() { return rno; }
	UINT32 getSRegId(UINT32 id)
	{
		if(id==0) {
			return regS;
		}
		return UINT_MAX;
	};
	UINT32 getDRegId() {return regT;}
	INT16 getImm() { return imm; }
private:
	static const UINT32 rno = 1;
	UINT32 regS;
	UINT32 regT;
	INT16 imm;
	std::string printStr;
};

class SLTU : public Instruction {
public:
	SLTU(UINT32 val, INT32 pc);
	std::string& getPrintStr() { return printStr;}
	OpCode type() { return OPSLTU;}
	UINT32 regCount() { return rno; }
	UINT32 getSRegId(UINT32 id)
	{
		if(0==id) {
			return regS;
		}
		else if(1==id) {
			return regT;
		}
		return UINT_MAX;
	};
	UINT32 getDRegId() {return regD;}
private:
	static const UINT32 rno = 2;
	UINT32 regS;
	UINT32 regT;
	UINT32 regD;
	std::string printStr;
};

class SLL : public Instruction {
public:
	SLL(UINT32 val, INT32 pc);
	std::string& getPrintStr() { return printStr;}
	OpCode type() { return OPSLL;}
	UINT32 regCount() { return rno; }
	UINT32 getSRegId(UINT32 id)
	{
		if(0==id) {
			return regT;
		}
		return UINT_MAX;
	};
	UINT32 getDRegId() {return regD;}
	UINT32 getSA() { return sa; }
private:
	static const UINT32 rno = 1;
	UINT32 regD;
	UINT32 regT;
	UINT32 sa;
	std::string printStr;
};

class SRL : public Instruction {
public:
	SRL(UINT32 val, INT32 pc);
	std::string& getPrintStr() { return printStr;}
	OpCode type() { return OPSRL;}
	UINT32 regCount() { return rno; }
	UINT32 getSRegId(UINT32 id)
	{
		if(0==id) {
			return regT;
		}
		return UINT_MAX;
	};
	UINT32 getDRegId() {return regD;}
	UINT32 getSA() { return sa; }
private:
	static const UINT32 rno = 1;
	UINT32 regT;
	UINT32 regD;
	UINT32 sa;
	std::string printStr;
};

class SRA : public Instruction {
public:
	SRA(UINT32 val, INT32 pc);
	std::string& getPrintStr() { return printStr;}
	OpCode type() { return OPSRA;}
	UINT32 regCount() { return rno; }
	UINT32 getSRegId(UINT32 id)
	{
		if(0==id) {
			return regT;
		}
		return UINT_MAX;
	};
	UINT32 getDRegId() {return regD;}
	INT16 getSA() { return sa; }
private:
	static const UINT32 rno = 1;
	UINT32 regT;
	UINT32 regD;
	UINT32 sa;
	std::string printStr;
};

class SUB : public Instruction {
public:
	SUB(UINT32 val, INT32 pc);
	std::string& getPrintStr() { return printStr;}
	OpCode type() { return OPSUB;}
	UINT32 regCount() { return rno; }
	UINT32 getSRegId(UINT32 id)
	{
		if(0==id) {
			return regS;
		}
		else if(1==id) {
			return regT;
		}
		return UINT_MAX;
	};
	UINT32 getDRegId() {return regD;}
private:
	static const UINT32 rno = 2;
	UINT32 regS;
	UINT32 regT;
	UINT32 regD;
	std::string printStr;
};

class SUBU : public Instruction {
public:
	SUBU(UINT32 val, INT32 pc);
	std::string& getPrintStr() { return printStr;}
	OpCode type() { return OPSUBU;}
	UINT32 regCount() { return rno; }
	UINT32 getSRegId(UINT32 id)
	{
		if(0==id) {
			return regS;
		}
		else if(1==id) {
			return regT;
		}
		return UINT_MAX;
	};
	UINT32 getDRegId() {return regD;}
private:
	static const UINT32 rno = 2;
	UINT32 regS;
	UINT32 regT;
	UINT32 regD;
	std::string printStr;
};

class AND : public Instruction {
public:
	AND(UINT32 val, INT32 pc);
	std::string& getPrintStr() { return printStr;}
	OpCode type() { return OPAND;}
	UINT32 regCount() { return rno; }
	UINT32 getSRegId(UINT32 id)
	{
		if(0==id) {
			return regS;
		}
		else if(1==id) {
			return regT;
		}
		return UINT_MAX;
	};
	UINT32 getDRegId() {return regD;}
private:
	static const UINT32 rno = 2;
	UINT32 regS;
	UINT32 regT;
	UINT32 regD;
	std::string printStr;
};

class OR : public Instruction {
public:
	OR(UINT32 val, INT32 pc);
	std::string& getPrintStr() { return printStr;}
	OpCode type() { return OPOR;}
	UINT32 regCount() { return rno; }
	UINT32 getSRegId(UINT32 id)
	{
		if(0==id) {
			return regS;
		}
		else if(1==id) {
			return regT;
		}
		return UINT_MAX;
	};
	UINT32 getDRegId() {return regD;}
private:
	static const UINT32 rno = 2;
	UINT32 regS;
	UINT32 regT;
	UINT32 regD;
	std::string printStr;
};

class XOR : public Instruction {
public:
	XOR(UINT32 val, INT32 pc);
	std::string& getPrintStr() { return printStr;}
	OpCode type() { return OPXOR;}
	UINT32 regCount() { return rno; }
	UINT32 getSRegId(UINT32 id)
	{
		if(0==id) {
			return regS;
		}
		else if(1==id) {
			return regT;
		}
		return UINT_MAX;
	};
	UINT32 getDRegId() {return regD;}
private:
	static const UINT32 rno = 2;
	UINT32 regS;
	UINT32 regT;
	UINT32 regD;
	std::string printStr;
};

class NOR : public Instruction {
public:
	NOR(UINT32 val, INT32 pc);
	std::string& getPrintStr() { return printStr;}
	OpCode type() { return OPNOR;}
	UINT32 regCount() { return rno; }
	UINT32 getSRegId(UINT32 id)
	{
		if(0==id) {
			return regS;
		}
		else if(1==id) {
			return regT;
		}
		return UINT_MAX;
	};
	UINT32 getDRegId() {return regD;}
private:
	static const UINT32 rno = 2;
	UINT32 regS;
	UINT32 regT;
	UINT32 regD;
	std::string printStr;
};

class NOP : public Instruction {
public:
	NOP(UINT32 val, INT32 pc);
	std::string& getPrintStr() { return printStr;}
	OpCode type() { return OPNOP;}
private:
	std::string printStr;
};


}
#endif //MIPS_H_
