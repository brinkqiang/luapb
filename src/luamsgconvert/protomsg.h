
#ifndef __PROTOMSG_H_INCLUDE__
#define __PROTOMSG_H_INCLUDE__

#include <map>
#include "google/protobuf/descriptor.h"
#include "google/protobuf/compiler/importer.h"

typedef struct MsgSize 
{
    uint8_t MsgSize1;
    uint8_t MsgSize2;
    uint8_t MsgSize3;
}MsgSize;

typedef struct MsgHeader
{
    MsgSize size;
    uint16_t MsgID;
    uint32_t MsgCRC;
    uint32_t MsgPreseve;
}MsgHeader;

class CProtoMsg
{
public:
    typedef std::map<int, std::string> MapID2Name;
    typedef std::map<int, std::string>::iterator MapID2MsgIt;

    typedef std::map<std::string, int> MapName2ID;
    typedef std::map<std::string, int>::iterator MapName2IDIt;

    CProtoMsg();
    virtual ~CProtoMsg();

    void Import(google::protobuf::compiler::Importer& oImporter);

    void DoActionMsgID(google::protobuf::compiler::Importer& oImporter, std::string strMsgEnumName, std::string strSplitName, std::string strPackageName);

    void Reset();
    void Register(int nMsgID, std::string strMsg);

    std::string ID2Name(int nMsgID);
    int Name2ID(std::string strMsg);
private:
    MapID2Name m_mapID2Name;
    MapName2ID m_mapName2ID;

    const google::protobuf::FileDescriptor* m_poFile;
};

#endif // __PROTOMSG_H_INCLUDE__