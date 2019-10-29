#pragma once
#include <cinttypes>
#include <cstring>
#include <string>

/* ============================== TCP HEADER ===============================
0                   1                   2                   3
0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+---------------+---------------+---------------+---+-+-+-+-+-+-+
|                                               | C |           |
|                                               | T |ENCRYPTION |
|     Payload Data length(LittleEndian)         | R |   WORD    |
|                                               | L |           |
|                (24 bit)                       |(2)|    (6)    |
+---------------+---------------+---------------+-+-+-+-+-+-+-+-+
:                     Payload Data continued ...                :
+---------------------------------------------------------------+
*(CTRL bit):
00 : data
01 : ping
10 : pong
11 : close

*(describe encryption scheme):
encryption word: 0 ~ 31
*/
enum TcpControl {
  CTL_DATA = 0,
  CTL_PING,
  CTL_PONG,
  CTL_CLOSE,
};

struct TcpMsgHeader {
  uint32_t len : 24;
  uint32_t enc : 6;
  uint32_t ctl : 2;

  static size_t size() { return sizeof(TcpMsgHeader); }
};

struct CommandClose {
  CommandClose(uint16_t resson) {
    header.len = sizeof(resson);
    header.enc = 0;
    header.ctl = CTL_CLOSE;
    reason     = resson;
  }

  std::string encode() {
    std::string out;
    out.resize(header.size() + sizeof(reason));
    char* buff = const_cast<char*>(out.data());
    memcpy(buff, &header, header.size());
    memcpy(buff + header.size(), &reason, sizeof(reason));
    return out;
  }

  TcpMsgHeader header;
  uint16_t     reason;
};

struct CommandPing {
  CommandPing() {
    header.ctl = TcpControl::CTL_PING;
    header.enc = 0;
    header.len = 0;
  }

  std::string encode() { return std::string((char*)&header, header.size()); }

  TcpMsgHeader header;
};

struct CommandPong {
  CommandPong() {
    header.ctl = TcpControl::CTL_PONG;
    header.enc = 0;
    header.len = 0;
  }

  std::string encode() { return std::string((char*)&header, header.size()); }

  TcpMsgHeader header;
};

/* ============================== SERVER MESSAGE =============================== */
struct BaseServerHeader {
  BaseServerHeader()
    : cmdID(0)
    , data(nullptr)
    , dataLen(0) {}

  bool decode(const char* buff, size_t len) {
    if (len < sizeof(cmdID)) { return false; }
    cmdID   = *(uint32_t*)buff;
    dataLen = len - sizeof(cmdID);
    data    = buff + sizeof(cmdID);
    return true;
  }

  uint32_t    cmdID;
  const char* data;
  size_t      dataLen;
};

#define ECNRYPTWORD 0