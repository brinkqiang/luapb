#pragma once
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/message.h>

/* ============================== PROTOBUF =============================== */
typedef std::shared_ptr<google::protobuf::Message> PBMessage;
class PBHelper {
public:
  static PBMessage get_pb_message(const std::string& name) {
    const google::protobuf::Descriptor* descriptor =
      google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(name);
    if (!descriptor) return nullptr;
    const google::protobuf::Message* message =
      google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
    if (!message) return nullptr;
    return PBMessage(message->New());
  }

  static PBMessage decode_pb_message(const std::string& name, const char* data, size_t len) {
    auto msg = get_pb_message(name);
    if (msg && decode_pb_message(*msg, data, len)) {
      return msg;
    }
    return nullptr;
  }

  static bool decode_pb_message(google::protobuf::Message& msg, const char* data, size_t len) {
    google::protobuf::io::ArrayInputStream ais(data, (int)len);
    google::protobuf::io::CodedInputStream coded_input(&ais);
    return msg.ParseFromCodedStream(&coded_input);
  }

  static std::string encode_pb_message(google::protobuf::Message& msg) {
    std::string out;
    out.resize(msg.ByteSize());
    encode_pb_message_to(const_cast<char*>(out.data()), msg);
    return out;
  }

  static void encode_pb_message_to(char* to, const google::protobuf::Message& msg) {
    google::protobuf::io::ArrayOutputStream aos(to, msg.ByteSize());
    google::protobuf::io::CodedOutputStream cos(&aos);
    msg.SerializeToCodedStream(&cos);
  }
};