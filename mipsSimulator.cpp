#include <iostream>
#include "disassembler.h"
#include "processor.h"
#include <unistd.h>

using namespace std;

bool fileExists (const std::string& name);
bool isValidTrace(int argc, char* argv[], int& m, int& n);
bool hasValidRange(string& trace, int& m, int& n);
int main(int argc, char* argv[]) {
	if(argc<3) {
		cout<<"Invalid input argments\n";
		cout<<"argument format-> MIPSsim inputfilename outputfilename [-Tm:n] \n";
		cout<<"nputfilename - The file name of the binary input file\n";
		cout<<"Outputfilename - The file name to which to print the output\n";
		cout<<"-Tm:n - Optional argument to specify the start (m) and end (n) cycles of simulation tracing output.\n";
	}
	else {
		char base[] = "./";
		string ipfile(base);
		ipfile.append(argv[1]);

		string opfile(base);
		opfile.append(argv[2]);
		int m=-1,n=-1;
		if(fileExists(ipfile) && isValidTrace(argc, argv, m, n)) {
			Processor processor(ipfile, opfile, m, n);
			processor.init();
			processor.process();
		}
		else {
			cout<<"Invalid input\n";
		}
	}
	return 0;
}

bool fileExists (const std::string& name) {
	if (FILE *file = fopen(name.c_str(), "r")) {
		fclose(file);
		return true;
	} else {
		cout<<"Error: Invalid input file ";
		return false;
	}
}

bool isValidTrace(int argc, char* argv[], int& m, int& n) {
	if(argc==4) {
		string trace(argv[3]);
		if(trace[0] == '-' && trace[1]=='T' && hasValidRange(trace, m, n) ) {
			if( (m>0 && n>0 && m<=n)|| (m==0 && n==0) ) {
				return true;
			}
		}
		cout<<"Error: Invalid Trace input ";
		return false;
	}
	else if(argc>4) {
		return false;
	}
	return true;
}

bool hasValidRange(string& trace, int& m, int& n) {
	size_t colonPos = trace.find_first_of(":");
	string start(trace, 2, colonPos-2);
	string end(trace, colonPos+1);
	m = atoi(start.c_str());
	n = atoi(end.c_str());

	if( (m == 0 && trace[2]!='0') || (n==0 && trace[4] != '0')) {
		return false;
	}
	return true;
}

#if 0
#include <bitset>
std::bitset<5> b(base);
std::cout<<"base->"<<base<<std::endl;

std::bitset<5> t(regT);
std::cout<<"regT->"<<t<<std::endl;

std::bitset<16> o(offset);
std::cout<<"offset->"<<o<<std::endl;

#endif

