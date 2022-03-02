#ifndef PRIMATE_H
#define PRIMATE_H


#include "primate_bit.h"

namespace Primate {

/*
 * Primate Register Type
 */

/* template helpers */
template<typename T>
using RealType = std::conditional_t<std::is_enum_v<T>, std::underlying_type_t<T>, T>;

/* concepts / macros */
template<typename T>
concept Reg = ExtIntegral<typename T::data_t> && std::is_empty_v<T>;

template<typename T>
concept RegField = std::is_enum_v<T> && ExtIntegral<RealType<T>>;

#define RField(TYPE, NAME) enum class NAME : TYPE

/* functions */
#pragma primate extract
template<RegField F, ExtIntegral D>
F Extract(D data);

#pragma primate insert
template<RegField F, ExtIntegral D>
D Insert(D data, F field);

/*
 * Primate I / O
 */
// TODO: maybe put these in a macro and stick it in Program?

#pragma primate input_read
template<Reg T>
typename T::data_t InputRead();

#pragma primate input_seek
template<ExtIntegral N>
void InputSeek(N num_bytes);

#pragma primate output_write
template<Reg T>
void OutputWrite(typename T::data_t data);

#pragma primate output_seek
template<ExtIntegral N>
void OutputSeek(N num_bytes);

/*
 * Primate Program
 */

template<typename T>
concept Program = requires(T p) {
  /* types */
  // FIXME: would be nice if defining these types actually did anything, i.e.
  // made the I/O functions type safe
  Reg<typename T::InputT>;
  Reg<typename T::OutputT>;

  /* state */
  { p._done } -> std::same_as<bool&>;
  { p._init } -> std::same_as<bool&>;

  /* threads */
  { p.InitThread() } -> std::same_as<void>;
  { p.ServiceThread() } -> std::same_as<void>;
};

/*
 * Primate Launcher
 */
template<Program P>
void Launch(P& program) {
  while (!program._done) {
    if (!program._init) {
      program._init = false;
      program.InitThread();
    }
    else {
      program.ServiceThread();
    }
  }
}

};  /* namespace Primate */


#endif  /* PRIMATE_H */
