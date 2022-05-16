#include <iostream>
#include <fstream>
#include <string>
#include "primate.h"
#include "porc.h"

using namespace Primate;

static std::ifstream inFile("in.txt");

void printWideReg(PORC::InputT::data_t inReg) {
	for(int i = 0; i < segment_size; i++) {
		if(i%4 == 0) {
			printf(" [%d] ", i);
		}
		printf("%x", (int)(0x1 & (inReg >> (i))));
		
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
	for(int i = segment_bytes; i >= 0; i--) {
		strSegment[i] = (0xFF & (inReg >> (8*i)));
	}
	strSegment[segment_size + 1] = '\0';
	
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
		exit(0);
		return (PORC::InputT::data_t)0;
	}
	char strSegment[segment_bytes+1];
	int startPos = inFile.tellg();
	// printf("[pre-read] file at %d\n", (int)inFile.tellg());
	inFile.read(&strSegment[0], segment_bytes);
	strSegment[segment_bytes] = '\0';
	inFile.seekg(startPos);
	// printf("[post-read] file at %d\n", (int)inFile.tellg());
	PORC::InputT::data_t out = 0;

	// for(int i = 0; i < segment_size; i++) {
	// 	out = (out << 8) | ((int)strSegment[i]);
	// }
	// strSegment = "1234";
	out = str2reg(strSegment);
	// char str2[segment_size+1] = "1234";
	// out = str2reg(str2);
	// printf("out: %c, str: %s\n", out, strSegment);
	printf("str: %s, %d\n", strSegment, segment_size);
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
	printf("[IDX] %d\n", data);
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
	return (MSPM::Result::match_vec)((input & ((1 << MSPM::num_patterns) - 1)));
}

template<>
MSPM::Result::match_pos0 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos0)((input >> MSPM::num_patterns) & ((1 << segment_bytes_lg2) - 1));
}

template<>
MSPM::Result::match_pos1 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos1)((input >> (MSPM::num_patterns + segment_bytes_lg2)) & ((1 << segment_bytes_lg2) - 1));
}

template<>
MSPM::Result::match_pos2 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos2)((input >> (MSPM::num_patterns + 2*segment_bytes_lg2)) & ((1 << segment_bytes_lg2) - 1));
}

template<>
MSPM::Result::match_pos3 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos3)((input >> (MSPM::num_patterns + 3*segment_bytes_lg2)) & ((1 << segment_bytes_lg2) - 1));
}

template<>
MSPM::Result::match_pos4 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos4)((input >> (MSPM::num_patterns + 4*segment_bytes_lg2)) & ((1 << segment_bytes_lg2) - 1));
}

template<>
MSPM::Result::match_pos5 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos5)((input >> (MSPM::num_patterns + 5*segment_bytes_lg2)) & ((1 << segment_bytes_lg2) - 1));
}

template<>
MSPM::Result::match_pos6 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos6)((input >> (MSPM::num_patterns + 6*segment_bytes_lg2)) & ((1 << segment_bytes_lg2) - 1));
}

template<>
MSPM::Result::match_pos7 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos7)((input >> (MSPM::num_patterns + 7*segment_bytes_lg2)) & ((1 << segment_bytes_lg2) - 1));
}
template<>
MSPM::Result::match_pos8 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos8)((input >> (MSPM::num_patterns + 8*segment_bytes_lg2)) & ((1 << segment_bytes_lg2) - 1));
}

template<>
MSPM::Result::match_pos9 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos9)((input >> (MSPM::num_patterns + 9*segment_bytes_lg2)) & ((1 << segment_bytes_lg2) - 1));
}

template<>
MSPM::Result::match_pos10 Primate::Extract(MSPM::Result::data_t input) {
	printf("params: %d, %d\n", MSPM::num_patterns + 10*segment_bytes_lg2, ((1 << (segment_bytes_lg2+1)) - 1));
	return (MSPM::Result::match_pos10)((input >> (MSPM::num_patterns + 10*segment_bytes_lg2)) & ((1 << (segment_bytes_lg2+1)) - 1));
}

template<>
MSPM::Result::match_pos11 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos11)((input >> (MSPM::num_patterns + 11*segment_bytes_lg2)) & ((1 << segment_bytes_lg2) - 1));
}

template<>
MSPM::Result::match_pos12 Primate::Extract(MSPM::Result::data_t input) {
	printf("params: %d, %d\n", MSPM::num_patterns + 12*segment_bytes_lg2, ((1 << (segment_bytes_lg2+1)) - 1));
	return (MSPM::Result::match_pos12)((input >> (MSPM::num_patterns + 12*segment_bytes_lg2)) & ((1 << (segment_bytes_lg2+1)) - 1));
}

template<>
MSPM::Result::match_pos13 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos13)((input >> (MSPM::num_patterns + 13*segment_bytes_lg2)) & ((1 << segment_bytes_lg2) - 1));
}

template<>
MSPM::Result::match_pos14 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos14)((input >> (MSPM::num_patterns + 14*segment_bytes_lg2)) & ((1 << segment_bytes_lg2) - 1));
}

template<>
MSPM::Result::match_pos15 Primate::Extract(MSPM::Result::data_t input) {
	return (MSPM::Result::match_pos15)((input >> (MSPM::num_patterns + 15*segment_bytes_lg2)) & ((1 << segment_bytes_lg2) - 1));
}

template<>
Num::Result::num Primate::Extract(Num::Result::data_t input) {
	return (Num::Result::num)(input);
}

template<>
Num::Result::pos Primate::Extract(Num::Result::data_t input) {
	return (Num::Result::pos)((input << 1) >> (Num::num_bytes_lg2+1));
}

template<>
Num::Result::found Primate::Extract(Num::Result::data_t input) {
	return (Num::Result::found)(input >> (Num::num_bytes_lg2 + segment_bytes_lg2));
}
