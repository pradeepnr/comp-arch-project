all:
	g++ -std=gnu++11 mipsSimulator.cpp mips.cpp buffers.cpp instructionSet.cpp processor.cpp utility.cpp -o MIPSsim