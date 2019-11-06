
#ifndef PROTOIMPORTER_H_
#define PROTOIMPORTER_H_

#include "dmsingleton.h"

#include <string>

#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/compiler/importer.h>
#include "protomsg.h"


class ProtoImporterImpl {
  public:
    ProtoImporterImpl();
    virtual ~ProtoImporterImpl();
  public:
    bool Import(const std::string& strFileName);
    google::protobuf::Message* CreateMessage(const std::string& strTypeName);
    void ReleaseMessage(google::protobuf::Message* message);

    std::string ID2Name(int nMsgID);
    int Name2ID(std::string strMsg);
  public:
    google::protobuf::compiler::Importer m_oImporter;
    google::protobuf::DynamicMessageFactory m_oFactory;

    CProtoMsg m_oProtoMsg;
};

class ProtoImporterMgr : public TSingleton<ProtoImporterMgr> {
    friend class TSingleton<ProtoImporterMgr>;
  public:
    ProtoImporterMgr();
    virtual ~ProtoImporterMgr();

    bool Import(const std::string& strFileName);
    google::protobuf::Message* CreateMessage(const std::string& strTypeName);
    void ReleaseMessage(google::protobuf::Message* message);
    ProtoImporterImpl* GetImporter() {
        return m_poProtoImporter;
    }

    void SetImporter(ProtoImporterImpl* poProtoImporter) {
        m_poProtoImporter = poProtoImporter;
    }

    std::string ID2Name(int nMsgID);
    int Name2ID(std::string strMsg);

  private:
    ProtoImporterImpl* m_poProtoImporter;
};


#endif
