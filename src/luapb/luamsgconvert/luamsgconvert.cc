
// Copyright (c) 2018 brinkqiang (brink.qiang@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "luamsgconvert.h"

#include "protocol.h"
#include "proto/comm.pb.h"
#include "pbhelper.hpp"
#include "protoimporter.h"

static const int64_t PING_SEND_INTERVAL = 10 * 1000;
static const int64_t MAX_RECV_PACKAGE_LEN = 1024 * 10;
static const int64_t TIMEOUT_INTERVAL = 15 * 1000;

CLuaMsgConvert::CLuaMsgConvert()
{

}

CLuaMsgConvert::~CLuaMsgConvert()
{

}

void CLuaMsgConvert::Import(const std::string& strName)
{
    ProtoImporterMgr::Instance()->Import(strName);
}

//uint32 		msgType = 1;
//uint32 		sequence = 2;
//sint32		code = 3;
//uint64		session = 4;
//string		traceId = 5;
//string 		service = 11;		//proto package
//string 		message = 12;		//proto message
//bytes		    data = 13;
void CLuaMsgConvert::MsgConvert(uint64_t qwSessionID, int32_t cmd, const google::protobuf::Message& msg, std::string* out)
{
    std::string data;
    data.resize(sizeof(cmd) + msg.ByteSize());
    char* buff = const_cast<char*>(data.data());
    memcpy(buff, &cmd, sizeof(cmd));
    PBHelper::encode_pb_message_to(buff + sizeof(cmd), msg);

    TcpMsgHeader tcpHeader;
    tcpHeader.ctl = TcpControl::CTL_DATA;
    tcpHeader.enc = ECNRYPTWORD;
    tcpHeader.len = data.size();

    std::string temp;
    temp.resize(tcpHeader.size() + data.size());
    char* buffer = const_cast<char*>(temp.data());
    memcpy(buffer, &tcpHeader, tcpHeader.size());
    memcpy(buffer + tcpHeader.size(), data.data(), data.size());

    std::string strMsgByte;
    msg.SerializeToString(&strMsgByte);

    std::string strServiceName = msg.GetDescriptor()->file()->package();
    std::string strMsgName = msg.GetDescriptor()->name();

    protocol::comm_message xMsg;
    xMsg.set_msgtype(cmd);
    xMsg.set_sequence(++m_dwSequence);
    xMsg.set_code(0);
    xMsg.set_session(qwSessionID);
    xMsg.set_traceid("");
    xMsg.set_service(strServiceName);
    xMsg.set_message(strMsgName);
    xMsg.set_data(strMsgByte);

    xMsg.SerializeToString(out);
}

void CLuaMsgConvert::ping()
{
    CommandPing cmd;
    cmd.header.enc = ECNRYPTWORD;
    std::string buff = cmd.encode();

}

void CLuaMsgConvert::pong()
{
    CommandPong cmd;
    cmd.header.enc = ECNRYPTWORD;
    std::string buff = cmd.encode();
}

size_t CLuaMsgConvert::OnRecv(const char* data, size_t len) {
    // read head
    if (len < TcpMsgHeader::size()) { return 0; }
    TcpMsgHeader* header = (TcpMsgHeader*)data;
    if (header->len + header->size() > len) {
        if (header->len > MAX_RECV_PACKAGE_LEN) {
            return len;
        }
        return 0;
    }

    if (header->ctl == TcpControl::CTL_PING) {
        // ping
        pong();
    }
    else if (header->ctl == TcpControl::CTL_PONG) {
        // pong
        ping();
    }
    else if (header->ctl == TcpControl::CTL_CLOSE) {
        // close
        if (header->len == sizeof(uint16_t)) {
            uint16_t reason = *(uint16_t*)(data + header->size());
            //DebugLog(gCppLogger, "Receive close reason: {}", reason);
        }
    }
    else {
        // check encrypt
        if (header->enc != ECNRYPTWORD) {
            //WarnLog(gCppLogger, "Encrypt word different. Enc: {} Self: {}", header->enc, m_ecnryptWord);
            //close(1002);
            return len;
        }
        // data
        OnPackage(data + header->size(), header->len);

    }
    return header->size() + header->len;
}

size_t CLuaMsgConvert::OnPackage(const char* data, size_t len) {
    return len;
}