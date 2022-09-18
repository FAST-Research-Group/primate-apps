#ifndef PORC_H
#define PORC_H

#pragma once

#include <iostream>
#include <cstdint>
#include <concepts>
#include <typeinfo>
#include <string>
#include <vector>

#include "primate.h"
using namespace Primate;



/*
 * Types
 */

constexpr u32 segment_bytes_lg2 = 5;
constexpr u32 segment_bytes = 1 << segment_bytes_lg2;
constexpr u32 segment_size = segment_bytes * 8;
using SegmentIdx = UInt<segment_bytes_lg2 + 1>;
using Segment = UInt<segment_size>;

constexpr u32 port_bytes = 2;
constexpr u32 port_size = port_bytes * 8;
using Port = UInt<port_size>;

constexpr u32 idx_bytes = 8;
constexpr u32 idx_size = idx_bytes * 8;
using Idx = UInt<idx_size>;

Segment str2reg(char* strSegment);

void reg2str(Segment inReg, char* strSegment); 
void printWideReg(Segment inReg);



enum class ProtocolPort : Port {
  DNS   = 53,
  HTTP  = 80,
  //HTTP2 = 80,  // TODO: @Minya, HTTP / HTTP/2 use the same port?
  POP3  = 110
};

#pragma primate reg
struct PORCInput {
  using data_t = Segment;
  RField(Segment, data);
};
static_assert(Reg<PORCInput>);

#pragma primate reg
struct Metadata {
  using data_t = Segment;
  RField(Port, port);
};
static_assert(Reg<Metadata>);

#pragma primate reg
struct PORCOutput {
  using data_t = Idx;
  RField(Idx, idx);
};
static_assert(Reg<PORCOutput>);

struct DNS {
  #pragma primate reg
  struct Header {
    using data_t = Segment;
    RField(u16, qd_cnt);
    RField(u16, an_cnt);
    RField(u16, ns_cnt);
    RField(u16, ar_cnt);
  };
  static_assert(Reg<Header>);
  
  #pragma primate reg
  struct Record {
    using data_t = Segment;
    RField(u16, type);
  };
  static_assert(Reg<Record>);
  
  #pragma primate reg
  struct Byte {
    using data_t = Segment;
    RField(u8, byte);
  };
  static_assert(Reg<Byte>);
};


/*
 * Blue function objects
 */
// I could not put this inside the MSPM and i've wasted an hour on this.
static std::vector<std::string> patternStore = {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""};

struct MSPM {
  /* types */
  static constexpr u32 num_patterns_lg2 = 4;
  static constexpr u32 num_patterns = 1 << num_patterns_lg2;
  using pattern_idx_t = UInt<num_patterns_lg2>;
  using pattern_vec_t = UInt<num_patterns>;

  static constexpr u32 pattern_bytes_lg2 = 6;
  static constexpr u32 pattern_bytes = 1 << pattern_bytes_lg2;
  static constexpr u32 pattern_size = pattern_bytes * 8;
  using pattern_byte_idx_t = UInt<pattern_bytes_lg2>;
  using pattern_t = UInt<pattern_size>;

  enum class Op : u2 {
      reset     = 0,
      configure = 1,
      match     = 2
  };

  #pragma primate reg
  struct Result {
    using data_t = UInt<num_patterns + num_patterns * segment_bytes_lg2>;
    RField(pattern_vec_t, match_vec);
    RField(SegmentIdx, match_pos0);
    RField(SegmentIdx, match_pos1);
    RField(SegmentIdx, match_pos2);
    RField(SegmentIdx, match_pos3);
    RField(SegmentIdx, match_pos4);
    RField(SegmentIdx, match_pos5);
    RField(SegmentIdx, match_pos6);
    RField(SegmentIdx, match_pos7);
    RField(SegmentIdx, match_pos8);
    RField(SegmentIdx, match_pos9);
    RField(SegmentIdx, match_pos10);
    RField(SegmentIdx, match_pos11);
    RField(SegmentIdx, match_pos12);
    RField(SegmentIdx, match_pos13);
    RField(SegmentIdx, match_pos14);
    RField(SegmentIdx, match_pos15);
  };
  static_assert(Reg<Result>);
  /* blue functions */
  #pragma primate blue MSPM 1 1
  static Result::data_t fu(Op op, pattern_idx_t idx, pattern_byte_idx_t len, Segment str) {

    switch(op) {
      case Op::reset: {
        // patternStore = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        patternStore.clear();
        for(int i = 0; i < num_patterns; i++) {
          patternStore.push_back("");
        }
        break;
      }
      case Op::configure: {
        char cstr[pattern_size+1];
        reg2str(str, cstr);
        //printf("[configure]loading %s into index %d\n", cstr, idx);
        patternStore[idx] = cstr;
        break;
      }
      case Op::match: {
        // Convert the input segment into a cstr
        char cstr[segment_bytes+1];
        reg2str(str, cstr);
        cstr[segment_bytes] = '\0';
        // convert to a regular str so we can use stl magic
        std::string inStr = cstr;
        Result::data_t out = 0;
        for(int i = 0; i < num_patterns; i++) {
            // grab the stored string
            // printf("i: %d\n", i);
            // std::cout << patternStore[i] << "\n";
            std::string patternStr = patternStore[i];
            if(patternStr == "") {
              continue;
            }
            int pos = inStr.find(patternStr);
            if(pos != std::string::npos) {
              //printf("detected string %d, pos %d, %s in %s at \n", i, pos+patternStr.length(), patternStr.c_str(), inStr.c_str());
              //printf("place at %d \n", (num_patterns + i*segment_bytes_lg2));
              out |= 1 << i;
              // this is hacky
              //printf("WERJWEIOPRJWE\n");
              //printf("\n");
              //printWideReg((Segment)out);
              out |= (((Result::data_t)((pos + patternStr.length()))) << (num_patterns + i*segment_bytes_lg2));
              //printf("P{L:S\n");
              //printWideReg((Segment)(((Result::data_t)((pos + patternStr.length()))) << (num_patterns + i*segment_bytes_lg2)));
              //printf("\n");
            }
        }
        return out;
        break;
      }

      default: break;
    }
    return 0;
  }

  /* helper functions */
  static void configure(pattern_idx_t pattern_idx, pattern_byte_idx_t len, pattern_t pattern) {
    fu(Op::configure, pattern_idx, len, (Segment) pattern);
  }

  static Result::data_t match(Segment word) {
    Result::data_t match = fu(Op::match, 0, 0, word);
    return match;
  }

  constexpr static pattern_t str_to_pattern(const char* str) {
    pattern_t pattern = 0;

    for (size_t i = 0; i < pattern_bytes; ++i) {
      if (str[i] == '\0')
        break;
      pattern |= ((pattern_t) str[i]) << (i * 8);
    }

    return pattern;
  }
};

struct Num {
  /* types */
  static constexpr u32 num_bytes_lg2 = 6;
  static constexpr u32 num_bytes = 1 << num_bytes_lg2;
  using num_t = UInt<num_bytes>;
  using found_t = UInt<1>;

  #pragma primate reg
  struct Result {
    //TODO: Ask alex about typing
    using data_t = UInt<num_bytes_lg2+segment_bytes_lg2+1>;
    RField(num_t, num);
    RField(SegmentIdx, pos);
    RField(found_t, found);
  };
  static_assert(Reg<Result>);

  /* blue functions */
  #pragma primate blue latch_num_ascii 1 1
  static Result::data_t convert_ascii(Segment str) {
    // This is the worst possible way to do this :)
    Result::data_t out = 0;
    int pow = 0;
    char cstr[MSPM::pattern_size+1];
    cstr[MSPM::pattern_size] = '\0';
    reg2str(str, cstr);
    // printf("[ascii_convert] %s\n", cstr);
    for(int i = 0; i <= 10; i++) {
      if(cstr[i] < '0' || cstr[i] > '9') {
        cstr[i] = 0;
        break;
      } else {
        out = out*10 + (cstr[i]-'0');
      }
    }
    return out;
  }

   /* blue functions */
  #pragma primate blue find_num 1 1
  static Result::data_t find_num(Segment str) {
    // This is the worst possible way to do this :)
    Result::data_t out = 0;
    int pow = 0;
    char cstr[MSPM::pattern_size+1];
    cstr[MSPM::pattern_size] = '\0';
    reg2str(str, cstr);
    // printf("[ascii_find] %s\n", cstr);
    for(int i = 0; i < segment_bytes; i++) {
      if(cstr[i] >= '0' && cstr[i] <= '9') {
        out = (((Segment)i) << num_bytes_lg2) | (((Segment)1 )<< (num_bytes_lg2 + segment_bytes_lg2));
        // printf("[ascii pos] ");
        // printWideReg(out);
        // printf("\n");
        return out;
      }
    } 
    return out;
  }


};


/*
 * Program
 */

#pragma primate model stream
struct PORC {
  /* types */
  using InputT = PORCInput;
  using OutputT = PORCOutput;

  /* state */
  bool _done = false;
  bool _init = true;

  /* threads */
  void InitThread();
  void ServiceThread();
};
static_assert(Program<PORC>);
#endif