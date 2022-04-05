#include <iostream>
#include <cstdint>
#include <concepts>
#include <typeinfo>


#include "primate.h"
using namespace Primate;


#pragma primate reg
struct Packet {
  using data_t = UInt<12 + 12>;
  RField(u12, a);
  RField(u12, b);
};
static_assert(Reg<Packet>);

#pragma primate model packet
struct Prog {
  /* types */
  using InputT = Packet;
  using OutputT = Packet;

  /* state */
  bool _done = false;
  bool _init = true;

  /* threads */
  void InitThread();
  void ServiceThread();
};
static_assert(Program<Prog>);

void Prog::InitThread() {}

void Prog::ServiceThread() {
  InputT::data_t input = InputRead<InputT>();
  u12 val = (u12) Extract<Packet::a>(input);
  val += 1;
  std::cout << (uint32_t)val << std::endl;
  Packet::data_t output = Insert(input, (Packet::a) val);
  OutputWrite<OutputT>(output);
}


int main() {
  Prog p{};
  Launch(p);
}
