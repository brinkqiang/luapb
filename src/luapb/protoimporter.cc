
#include "protoimporter.h"

#include <stdio.h>
#include "dmutil.h"

class ProtobufLibrary
{
public:
    ProtobufLibrary()
    {
        GOOGLE_PROTOBUF_VERIFY_VERSION;
    };
    virtual ~ProtobufLibrary()
    {
        // fixed memory leak
        google::protobuf::ShutdownProtobufLibrary();
    }
};

static ProtobufLibrary _protobuf_library;

class MyMultiFileErrorCollector : public google::protobuf::compiler::MultiFileErrorCollector
{	
public:
	virtual void AddError(const std::string& filename, int line, int column,
        const std::string& message)
	{
		fprintf(stderr, "%s:%d:%d:%s\n", filename.c_str(), line, column, message.c_str());
	}

    virtual void AddWarning(const std::string& filename, int line, int column,
        const std::string& message)
    {
        fprintf(stdout, "%s:%d:%d:%s\n", filename.c_str(), line, column, message.c_str());
    }
};

static MyMultiFileErrorCollector errorCollector;
static google::protobuf::compiler::DiskSourceTree sourceTree;

ProtoImporterImpl::ProtoImporterImpl():
		m_oImporter(&sourceTree, &errorCollector)
{

}

ProtoImporterImpl::~ProtoImporterImpl()
{
}

bool ProtoImporterImpl::Import(const std::string& strFileName)
{
	const  google::protobuf::FileDescriptor* filedescriptor = m_oImporter.Import(strFileName);
	if (!filedescriptor)
	{
		fprintf(stderr, "import (%s) file descriptor error\n", strFileName.c_str());
		return false;
	}
	return true;
}

google::protobuf::Message* ProtoImporterImpl::CreateMessage(const std::string& strTypeName)
{
	google::protobuf::Message* message = NULL;
	const google::protobuf::Descriptor* descriptor = m_oImporter.pool()->FindMessageTypeByName(strTypeName);
	if (descriptor)
	{
		const google::protobuf::Message* prototype = m_oFactory.GetPrototype(descriptor);
		if (prototype)
		{
			message = prototype->New();
		}
	}
	return message;
}

ProtoImporter::ProtoImporter()
{
    std::string strRoot = DMGetRootPath();
    std::string strProtoPath = strRoot + PATH_DELIMITER_STR + "proto";
    std::string strProtoPath2 = strRoot + PATH_DELIMITER_STR + ".." + PATH_DELIMITER_STR + "proto";

    sourceTree.MapPath("", strRoot);
    sourceTree.MapPath("", strProtoPath);
    sourceTree.MapPath("", strProtoPath2);

    printf("[ProtoImporter] protopath:%s\n", strRoot.c_str());
    printf("[ProtoImporter] protopath:%s\n", strProtoPath.c_str());
    printf("[ProtoImporter] protopath:%s\n", strProtoPath2.c_str());
    SetImporter(new ProtoImporterImpl());
}

ProtoImporter::~ProtoImporter()
{

}

bool ProtoImporter::Import(const std::string& strFileName)
{
    ProtoImporterImpl* poProtoImporter = GetImporter();
    if (NULL != poProtoImporter)
    {
        delete poProtoImporter;
        SetImporter(NULL);
    }

    SetImporter(new ProtoImporterImpl());
    poProtoImporter = GetImporter();
 
    if (NULL == poProtoImporter)
    {
        return false;
    }

    return poProtoImporter->Import(strFileName);
}

google::protobuf::Message* ProtoImporter::CreateMessage(const std::string& strTypeName)
{
    return m_poProtoImporter->CreateMessage(strTypeName);
}
