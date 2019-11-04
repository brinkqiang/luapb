
#include "protomsg.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/compiler/importer.h"
#include "dmstrtk.hpp"
CProtoMsg::CProtoMsg()
{

}

CProtoMsg::~CProtoMsg()
{
}

void CProtoMsg::Import(google::protobuf::compiler::Importer& oImporter)
{
    Reset();

    DoActionMsgID(oImporter, "MSGID", "MSGID_", "net");
}

void CProtoMsg::DoActionMsgID(google::protobuf::compiler::Importer& oImporter, std::string strMsgEnumName, std::string strSplitName, std::string strPackageName)
{
    const google::protobuf::EnumDescriptor* enumDesc = oImporter.pool()->FindEnumTypeByName(strMsgEnumName);
    if (NULL == enumDesc)
    {
        return;
    }

    for (int j = 0; j < enumDesc->value_count(); j++)
    {
        const google::protobuf::EnumValueDescriptor* enumValue = enumDesc->value(j);
        if (NULL == enumDesc)
        {
            continue;
        }
        std::string strName = enumValue->full_name();
        std::string strMsgName;
        strtk::replace_pattern(strName, strSplitName, "", strMsgName);
        strMsgName = strPackageName + "." + strMsgName;

        const google::protobuf::Descriptor* Descriptor = oImporter.pool()->FindMessageTypeByName(strMsgName);
        if (NULL == Descriptor)
        {
            continue;
        }

        Register(enumValue->number(), strMsgName);
    }
}

void CProtoMsg::Reset()
{
    m_mapID2Name.clear();
    m_mapName2ID.clear();
}

void CProtoMsg::Register(int nMsgID, std::string strMsg)
{
    m_mapID2Name[nMsgID] = strMsg;
    m_mapName2ID[strMsg] = nMsgID;
}

std::string CProtoMsg::ID2Name(int nMsgID)
{
    MapID2MsgIt it = m_mapID2Name.find(nMsgID);
    if (it == m_mapID2Name.end())
    {
        return std::string();
    }

    return it->second;
}

int CProtoMsg::Name2ID(std::string strMsg)
{
    MapName2IDIt it = m_mapName2ID.find(strMsg);
    if (it == m_mapName2ID.end())
    {
        return 0;
    }

    return it->second;
}
