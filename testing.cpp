#include <iostream>
#include <fstream>
#include <string>
#include "primate.h"
#include "porc.h"

using namespace Primate;

static std::ifstream inFile("in.txt");

void printWideReg(PORC::InputT::data_t inReg) {
	for(int i = 0; i < segment_size; i++) {
		printf("%c", (int)(0xFF & (inReg >> (8*i))));
	}
}

Segment str2reg(char* strSegment) {
	Segment out = 0;
	for(int i = segment_size; i >= 0; i--) {
		out = (out << 8) | ((int)strSegment[i]);
	}
	return out;
}

void reg2str(Segment inReg, char* strSegment) {
	for(int i = segment_size; i >= 0; i--) {
		strSegment[i] = (0xFF & (inReg >> (8*i)));
	}
}



//Input Testing Functions
template<>
PORC::InputT::data_t Primate::InputRead<PORC::InputT>() {
	if(!inFile.is_open()) {
		printf("opening file\n");
		inFile.open("in.txt");
	}
	if(!inFile){ 
		printf("AHHHHH\n");
		return (PORC::InputT::data_t)0;
	}
	char strSegment[segment_size+1];
	int startPos = inFile.tellg();
	printf("[pre-read] file at %d\n", (int)inFile.tellg());
	inFile.read(&strSegment[0], segment_size);
	strSegment[segment_size] = '\0';
	inFile.seekg(startPos);
	printf("[post-read] file at %d\n", (int)inFile.tellg());
	PORC::InputT::data_t out = 0;

	// for(int i = 0; i < segment_size; i++) {
	// 	out = (out << 8) | ((int)strSegment[i]);
	// }
	out = str2reg(strSegment);
	char str2[segment_size+1];
	// printf("out: %c, str: %s\n", out, strSegment);
	printf("out: ");
	printWideReg(out);
	reg2str(out, str2);
	printf(", str: %s\n", str2);
	// printf("[post-seek] file at %d\n", (int)inFile.tellg());
	return out;
}

template<>
void Primate::InputSeek(u32 num_bytes) {
	if(!inFile.is_open()) {
		printf("opening file\n");
		inFile.open("in.txt");
	}
	printf("[pre-seek] file at %d\n", (int)inFile.tellg());
	inFile.seekg(inFile.tellg()+num_bytes);
	printf("[post-seek] file at %d\n", (int)inFile.tellg());

}

//Output Testing Functions
template<>
void Primate::OutputWrite<PORCOutput>(PORCOutput::data_t data) {
	
}

template<>
void Primate::OutputSeek(u32 num_bytes) {

}

//Special extraction functions

template<>
Metadata::port Primate::Extract(PORC::InputT::data_t input) {
	ProtocolPort outPort = ProtocolPort::HTTP;
	return (Metadata::port) outPort;
}

template<>
DNS::Header::qd_cnt Primate::Extract(PORC::InputT::data_t input) {
	return (DNS::Header::qd_cnt)0;
}

template<>
DNS::Header::an_cnt Primate::Extract(PORC::InputT::data_t input) {
	return (DNS::Header::an_cnt)0;
}

template<>
DNS::Header::ns_cnt Primate::Extract(PORC::InputT::data_t input) {
	return (DNS::Header::ns_cnt)0;
}

template<>
DNS::Header::ar_cnt Primate::Extract(PORC::InputT::data_t input) {
	return (DNS::Header::ar_cnt)0;
}

template<>
DNS::Byte::byte Primate::Extract(PORC::InputT::data_t input) {
	return (DNS::Byte::byte)0;
}

template<>
DNS::Record::type Primate::Extract(PORC::InputT::data_t input) {
	return (DNS::Record::type)0;
}


template<>
MSPM::Result::match_vec Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_vec)0;
}

template<>
MSPM::Result::match_pos0 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos0)0;
}

template<>
MSPM::Result::match_pos1 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos1)0;
}

template<>
MSPM::Result::match_pos2 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos2)0;
}

template<>
MSPM::Result::match_pos3 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos3)0;
}

template<>
MSPM::Result::match_pos4 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos4)0;
}

template<>
MSPM::Result::match_pos5 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos5)0;
}

template<>
MSPM::Result::match_pos6 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos6)0;
}

template<>
MSPM::Result::match_pos7 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos7)0;
}

template<>
MSPM::Result::match_pos8 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos8)0;
}

template<>
MSPM::Result::match_pos9 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos9)0;
}

template<>
MSPM::Result::match_pos10 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos10)0;
}

template<>
MSPM::Result::match_pos11 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos11)0;
}

template<>
MSPM::Result::match_pos12 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos12)0;
}

template<>
MSPM::Result::match_pos13 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos13)0;
}

template<>
MSPM::Result::match_pos14 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos14)0;
}

template<>
MSPM::Result::match_pos15 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos15)0;
}

template<>
Num::Result::num Primate::Extract(Num::Result::data_t input) {
	return (Num::Result::num)0;
}

template<>
Num::Result::pos Primate::Extract(Num::Result::data_t input) {
	return (Num::Result::pos)0;
}


