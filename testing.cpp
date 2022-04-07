#include <iostream>
#include "primate.h"
#include "porc.h"

using namespace Primate;

template<>
PORC::InputT::data_t InputRead();

PORC::InputT::data_t InputRead() {
	// static std::ifstream inFile;
	// if(!inFile.is_open()) {
	// 	inFile.open("in.txt");
	// }
	return 0;
}

template<>
void InputSeek<PORC::InputT>();
void InputSeek() {
	
}