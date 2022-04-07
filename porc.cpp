#include "porc.h"

void PORC::InitThread() {
    MSPM::configure(0, 2, MSPM::str_to_pattern("\r\n"));
    MSPM::configure(1, 1, MSPM::str_to_pattern("\n"));
    /* POP3 */
    MSPM::configure(2, 4, MSPM::str_to_pattern("USER"));
    MSPM::configure(3, 4, MSPM::str_to_pattern("PASS"));
    MSPM::configure(4, 4, MSPM::str_to_pattern("QUIT"));
    MSPM::configure(5, 4, MSPM::str_to_pattern("STAT"));
    MSPM::configure(6, 4, MSPM::str_to_pattern("LIST"));
    MSPM::configure(7, 4, MSPM::str_to_pattern("RETR"));
    MSPM::configure(8, 4, MSPM::str_to_pattern("DELE"));
    MSPM::configure(9, 4, MSPM::str_to_pattern("NOOP"));
    /* HTTP */
    // TODO: is there definitely a space here?
    MSPM::configure(10, 16, MSPM::str_to_pattern("content_length: "));
    MSPM::configure(11, 26, MSPM::str_to_pattern("transfer_encoding: chunked"));
    MSPM::configure(12, 4, MSPM::str_to_pattern("\r\n\r\n"));
    // TODO: is there definitely a space here?
    MSPM::configure(13, 4, MSPM::str_to_pattern("0 \r\n"));
    /* DNS */
}

void PORC::ServiceThread() {
  // TODO: How does a flow end?

  InputT::data_t input = InputRead<InputT>();
  ProtocolPort port = (ProtocolPort) Extract<Metadata::port>(input);
  InputSeek(port_bytes);

  switch (port) {
    case ProtocolPort::DNS: {
      enum class State : u4 {
        header1    = 0,
        header2    = 1,
        question1  = 2,
        question2  = 3,
        record1    = 4,
        record2    = 5,
        record3    = 6,
        record4    = 7,
        record5    = 8,
        record6    = 9,
        parse_name = 10,
        end        = 11
      };

      State state = State::header1;
      State ret_state;
      Idx idx = 0;

      u16 qd_cnt;
      u16 an_cnt;
      u16 ns_cnt;
      u16 ar_cnt;

      while (true) {
        InputT::data_t input = InputRead<InputT>();
        SegmentIdx offset;

        switch (state) {
          case State::header1: {
            offset = 4;
            state = State::header2;
          }
          break;

          case State::header2: {
            u16 qd_cnt = (u16) Extract<DNS::Header::qd_cnt>(input);
            u16 an_cnt = (u16) Extract<DNS::Header::an_cnt>(input);
            u16 ns_cnt = (u16) Extract<DNS::Header::ns_cnt>(input);
            u16 ar_cnt = (u16) Extract<DNS::Header::ar_cnt>(input);
            u16 loops;

            offset = 4 * 2;
            state = State::question1;
          }
          break;

          case State::question1: {
            ret_state = State::question2;
            state = State::parse_name;
          }
          break;

          case State::question2: {
            offset = 0;
            for (u16 i = 0; i < qd_cnt; ++i) {
              offset += 4;
            }

            state = State::record1;
          }
          break;

          case State::record1: {
            ret_state = State::record2;
            state = State::parse_name;
          }
          break;

          case State::record2: {
            offset = 0;
            for (u16 i = 0; i < an_cnt; ++i) {
              offset += 8;
              u16 type = (u16) Extract<DNS::Record::type>(input);
              offset += type;
            }

            state = State::record3;
          }
          break;

          case State::record3: {
            ret_state = State::record4;
            state = State::parse_name;
          }
          break;

          case State::record4: {
            offset = 0;
            for (u16 i = 0; i < an_cnt; ++i) {
              offset += 8;
              u16 type = (u16) Extract<DNS::Record::type>(input);
              offset += type;
            }

            state = State::record5;
          }
          break;

          case State::record5: {
            ret_state = State::record6;
            state = State::parse_name;
          }
          break;

          case State::record6: {
            offset = 0;
            for (u16 i = 0; i < an_cnt; ++i) {
              offset += 8;
              u16 type = (u16) Extract<DNS::Record::type>(input);
              offset += type;
            }

            state = State::end;
          }
          break;

          case State::end: {
            OutputWrite<OutputT>(idx);
            OutputSeek(idx_bytes);

            state = State::header1;
          }
          break;

          case State::parse_name: {
            u8 byte = (u8) Extract<DNS::Byte::byte>(input);

            if (byte != 0) {
              if (byte < 0x70) {
                offset = byte + 1;
              }
              else {
                offset = 3;
              }
            }
            else {
              offset = 1;
              state = ret_state;
            }
          }
          break;
        }

        idx += offset;
        InputSeek(offset);
      }
    }
    break;

    case ProtocolPort::HTTP: {
      enum class State : u3 {
        mode     = 0,
        m_len    = 1,
        m_len_hd = 2,
        m_chnk   = 3,
        end      = 4
      };

      State state = State::mode;
      Idx idx = 0;

      while (true) {
        InputT::data_t input = InputRead<InputT>();
        Idx content_length = 0;
        SegmentIdx offset;

        switch (state) {
          case State::mode: {
            MSPM::Result::data_t match = MSPM::match(input);
            MSPM::pattern_vec_t match_vec = (MSPM::pattern_vec_t) Extract<MSPM::Result::match_vec>(match);

            if ((match_vec & (MSPM::pattern_vec_t) 0x1c00) != 0) {
              if ((match_vec & (MSPM::pattern_vec_t) (1 << 12)) != 0) {
                MSPM::Result::match_pos12 pos = Extract<MSPM::Result::match_pos12>(match);
                offset = (SegmentIdx) pos;

                state = State::end;
              }
              else if ((match_vec & (MSPM::pattern_vec_t) (1 << 10)) != 0) {
                MSPM::Result::match_pos10 pos = Extract<MSPM::Result::match_pos10>(match);
                offset = (SegmentIdx) pos;

                state = State::m_len;
              }
              else if ((match_vec & (MSPM::pattern_vec_t) (1 << 11)) != 0) {
                MSPM::Result::match_pos11 pos = Extract<MSPM::Result::match_pos11>(match);
                offset = (SegmentIdx) pos;

                state = State::m_chnk;
              }
            }
            else {
              offset = segment_bytes;
            }
          }
          break;

          case State::m_len: {
            Num::Result::data_t num_result = Num::convert_ascii(input);
            Num::Result::num num = Extract<Num::Result::num>(num_result);
            Num::Result::pos pos = Extract<Num::Result::pos>(num_result);
            content_length = (Num::num_t) num;
            offset = (SegmentIdx) pos;

            state = State::m_len_hd;
          }
          break;

          case State::m_len_hd: {
            MSPM::Result::data_t match = MSPM::match(input);
            MSPM::pattern_vec_t match_vec = (MSPM::pattern_vec_t) Extract<MSPM::Result::match_vec>(match);

            if ((match_vec & (MSPM::pattern_vec_t) (1 << 12)) != 0) {
              MSPM::Result::match_pos12 pos = Extract<MSPM::Result::match_pos12>(match);

              idx += content_length;
              offset = (SegmentIdx) pos;
              state = State::end;
            }
            else {
              offset = segment_bytes;
            }
          }
          break;

          case State::m_chnk: {
            MSPM::Result::data_t match = MSPM::match(input);
            MSPM::pattern_vec_t match_vec = (MSPM::pattern_vec_t) Extract<MSPM::Result::match_vec>(match);

            if ((match_vec & (MSPM::pattern_vec_t) (1 << 13)) != 0) {
              MSPM::Result::match_pos13 pos = Extract<MSPM::Result::match_pos13>(match);

              offset = (SegmentIdx) pos;
              state = State::end;
            }
          }
          break;

          case State::end: {
            OutputWrite<OutputT>(idx);
            OutputSeek(idx_bytes);

            state = State::mode;
          }
          break;
        }

        idx += offset;
        InputSeek(offset);
      }
    }
    break;

    case ProtocolPort::POP3: {
      enum class State : u2 {
        cmd  = 0,
        line = 1,
        end  = 2
      };

      State state = State::cmd;
      Idx idx = 0;

      while (true) {
        InputT::data_t input = InputRead<InputT>();
        SegmentIdx offset = 0;

        switch (state) {
          case State::cmd: {
            MSPM::Result::data_t match = MSPM::match(input);
            MSPM::pattern_vec_t match_vec = (MSPM::pattern_vec_t) Extract<MSPM::Result::match_vec>(match);

            if ((match_vec & (MSPM::pattern_vec_t) 0x3fc) != 0) {
              if ((match_vec & (MSPM::pattern_vec_t) (1 << 2)) != 0) {
                MSPM::Result::match_pos2 pos = Extract<MSPM::Result::match_pos2>(match);
                offset = (SegmentIdx) pos;
              }
              else if ((match_vec & (MSPM::pattern_vec_t) (1 << 3)) != 0) {
                MSPM::Result::match_pos3 pos = Extract<MSPM::Result::match_pos3>(match);
                offset = (SegmentIdx) pos;
              }
              else if ((match_vec & (MSPM::pattern_vec_t) (1 << 4)) != 0) {
                MSPM::Result::match_pos4 pos = Extract<MSPM::Result::match_pos4>(match);
                offset = (SegmentIdx) pos;
              }
              else if ((match_vec & (MSPM::pattern_vec_t) (1 << 5)) != 0) {
                MSPM::Result::match_pos5 pos = Extract<MSPM::Result::match_pos5>(match);
                offset = (SegmentIdx) pos;
              }
              else if ((match_vec & (MSPM::pattern_vec_t) (1 << 6)) != 0) {
                MSPM::Result::match_pos6 pos = Extract<MSPM::Result::match_pos6>(match);
                offset = (SegmentIdx) pos;
              }
              else if ((match_vec & (MSPM::pattern_vec_t) (1 << 7)) != 0) {
                MSPM::Result::match_pos7 pos = Extract<MSPM::Result::match_pos7>(match);
                offset = (SegmentIdx) pos;
              }
              else if ((match_vec & (MSPM::pattern_vec_t) (1 << 8)) != 0) {
                MSPM::Result::match_pos8 pos = Extract<MSPM::Result::match_pos8>(match);
                offset = (SegmentIdx) pos;
              }
              else if ((match_vec & (MSPM::pattern_vec_t) (1 << 9)) != 0) {
                MSPM::Result::match_pos9 pos = Extract<MSPM::Result::match_pos9>(match);
                offset = (SegmentIdx) pos;
              }

              state = State::line;
            }
            else {
              offset = segment_bytes;
            }
          }
          break;

          case State::line: {
            MSPM::Result::data_t match = MSPM::match(input);
            MSPM::pattern_vec_t match_vec = (MSPM::pattern_vec_t) Extract<MSPM::Result::match_vec>(match);

            if ((match_vec & (MSPM::pattern_vec_t) 0x1) != 0) {
              MSPM::Result::match_pos0 pos = Extract<MSPM::Result::match_pos0>(match);
              offset = (SegmentIdx) pos;

              state = State::end;
            }
            else {
              offset = segment_bytes;
            }
          }
          break;

          case State::end: {
            OutputWrite<OutputT>(idx);
            OutputSeek(idx_bytes);

            state = State::cmd;
          }
          break;
        }

        idx += offset;
        InputSeek(offset);
      }
    }
    break;
  }
}


/*
 * Launch
 */

int main() {
  PORC porc{};
  Launch(porc);
}
