
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

#include "luapb_module.h"
#include "protoimporter.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor_database.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/compiler/importer.h>

using namespace google::protobuf;

typedef struct tagluamsg {
    google::protobuf::Message* msg;
    bool isDelete; //是否要释放掉这个msg;
} lua_pbmsg;

typedef struct tagluarepeatedmsg {
    google::protobuf::Message* msg;
    google::protobuf::FieldDescriptor* field;
} lua_repeated_msg;

#define PB_MESSAGE "pb"
#define PB_MESSAGE_META "pb_meta"

#define PB_REPEATED_MESSAGE_META "pb_repeated_meta"

static int push_message(lua_State* L,
                        google::protobuf::Message* message,
                        bool del) {
    lua_pbmsg* tmp =
        static_cast<lua_pbmsg*>(lua_newuserdata(L, sizeof(lua_pbmsg)));

    if (tmp == NULL) {
        return 0;
    }

    tmp->msg = message;
    tmp->isDelete = del;

    luaL_getmetatable(L, PB_MESSAGE_META);
    lua_setmetatable(L, -2);
    return 1;
}

static int push_repeated_msg(lua_State* L,
                             Message* msg,
                             FieldDescriptor* field) {
    lua_repeated_msg* repeated =
        static_cast<lua_repeated_msg*>(lua_newuserdata(L, sizeof(lua_repeated_msg)));

    if (!repeated) {
        return 0;
    }

    repeated->msg = msg;
    repeated->field = field;

    luaL_getmetatable(L, PB_REPEATED_MESSAGE_META);
    lua_setmetatable(L, -2);
    return 1;
}

static int pb_repeated_add(lua_State* L) {
    lua_repeated_msg* repeated =
        (lua_repeated_msg*)luaL_checkudata(L, 1, PB_REPEATED_MESSAGE_META);

    Message* message = repeated->msg;

    if (!message) {
        luaL_argerror(L, 1, "pb_repeated_add, pb msg is nil");
        return 0;
    }

    FieldDescriptor* field = repeated->field;
    const Reflection* reflection = message->GetReflection();
    luaL_argcheck(L, field != NULL, 1, "pb_repeated_add, field is null");

    switch (field->type()) {
    case google::protobuf::FieldDescriptor::TYPE_MESSAGE: {
        Message* msg = reflection->AddMessage(message, field);
        return push_message(L, msg, false);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_INT32: {
        int32_t val = static_cast<int32_t>(luaL_checkinteger(L, 2));
        reflection->AddInt32(message, field, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_UINT32: {
        uint32_t val = static_cast<uint32_t>(luaL_checknumber(L, 2));
        reflection->AddUInt32(message, field, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_INT64: {
        int64_t val = static_cast<int64_t>(luaL_checknumber(L, 2));
        reflection->AddInt64(message, field, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_UINT64: {
        uint64_t val = static_cast<uint64_t>(luaL_checknumber(L, 2));
        reflection->AddUInt64(message, field, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_FLOAT: {
        float val = static_cast<float>(luaL_checknumber(L, 2));
        reflection->AddFloat(message, field, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_DOUBLE: {
        double val = static_cast<double>(luaL_checknumber(L, 2));
        reflection->AddDouble(message, field, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_STRING: {
        size_t strlen;
        const char* str = luaL_checklstring(L, 2, &strlen);
        reflection->AddString(message, field, str);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_BYTES: {
        size_t strlen;
        const char* str = luaL_checklstring(L, 2, &strlen);
        reflection->AddString(message, field, str);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_BOOL: {
        int val = static_cast<int>(luaL_checkinteger(L, 2));
        reflection->AddBool(message, field, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_ENUM: {
        int val = static_cast<int>(luaL_checkinteger(L, 2));
        reflection->AddEnumValue(message, field, val);
    }
    break;

    default:
        luaL_argerror(L, (2),
                      "pb_repeated_add field name type for add  is not support!!");
        return 0;
    }

    return 1;
}

static int pb_repeated_len(lua_State* L) {
    lua_repeated_msg* repeated =
        (lua_repeated_msg*)luaL_checkudata(L, 1, PB_REPEATED_MESSAGE_META);

    Message* message = repeated->msg;

    if (!message) {
        luaL_argerror(L, 1, "pb_repeated_len, pb msg is nil");
        return 0;
    }

    const Reflection* reflection = message->GetReflection();
    FieldDescriptor* field = repeated->field;
    luaL_argcheck(L, field != NULL, 1, "pb_repeated_len field not exist");

    int fieldsize = reflection->FieldSize(*message, field);
    lua_pushinteger(L, fieldsize);
    return 1;
}

static int pb_repeated_get(lua_State* L) {
    lua_repeated_msg* repeated =
        (lua_repeated_msg*)luaL_checkudata(L, 1, PB_REPEATED_MESSAGE_META);

    Message* message = repeated->msg;

    if (!message) {
        luaL_argerror(L, 1, "pb_repeated_get, pb msg is nil");
        return 0;
    }

    FieldDescriptor* field = repeated->field;
    const Reflection* reflection = message->GetReflection();
    luaL_argcheck(L, field != NULL, 1, "pb_repeated_get field not exist");

    int index = static_cast<int>(luaL_checkinteger(L, 2)) - 1;
    luaL_argcheck(L, index >= 0, 2, "pb_repeated_get index expected >= 1");

    switch (field->type()) {
    case google::protobuf::FieldDescriptor::TYPE_MESSAGE: {
        Message* msg = reflection->MutableRepeatedMessage(message, field, index);
        return push_message(L, msg, false);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_INT32: {
        lua_pushinteger(L, reflection->GetRepeatedInt32(*message, field, index));
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_UINT32: {
        lua_pushinteger(L, reflection->GetRepeatedUInt32(*message, field, index));
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_INT64: {
        lua_pushinteger(L, reflection->GetRepeatedInt64(*message, field, index));
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_UINT64: {
        lua_pushinteger(L, reflection->GetRepeatedUInt64(*message, field, index));
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_FLOAT: {
        lua_pushnumber(L, reflection->GetRepeatedFloat(*message, field, index));
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_DOUBLE: {
        lua_pushnumber(L, reflection->GetRepeatedDouble(*message, field, index));
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_STRING: {
        lua_pushstring(L, reflection->GetRepeatedString(*message, field, index).data());
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_BYTES: {
        std::string str = reflection->GetRepeatedString(*message, field, index);
        lua_pushlstring(L, str.data(), str.size());
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_BOOL: {
        lua_pushboolean(L, reflection->GetRepeatedBool(*message, field, index));
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_ENUM: {
        lua_pushinteger(L, reflection->GetRepeatedEnumValue(*message, field, index));
    }
    break;

    default:
        luaL_argerror(L, 0, "pb_repeated_get, field type for get not support!!!");
        return 0;
    }

    return 1;
}

static int pb_repeated_set(lua_State* L) {
    lua_repeated_msg* repeated =
        (lua_repeated_msg*)luaL_checkudata(L, 1, PB_REPEATED_MESSAGE_META);

    Message* message = repeated->msg;

    if (!message) {
        luaL_argerror(L, 1, "pb_repeated_set, pb msg is nil");
        return 0;
    }

    const Reflection* reflection = message->GetReflection();
    FieldDescriptor* field = repeated->field;
    luaL_argcheck(L, field != NULL, 1, "pb_repeated_set field not exist");

    int index = static_cast<int>(luaL_checkinteger(L, 2)) - 1;
    luaL_argcheck(L, index >= 0, 2, "pb_repeated_set index expected >= 1");

    switch (field->type()) {
    case google::protobuf::FieldDescriptor::TYPE_MESSAGE: {
        luaL_argerror(L, (2), "pb_repeated_set type_message for set not support!!!");
        return 0;
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_INT32: {
        int32_t val = static_cast<int32_t>(luaL_checkinteger(L, 3));
        reflection->SetRepeatedInt32(message, field, index, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_UINT32: {
        uint32_t val = static_cast<uint32_t>(luaL_checkinteger(L, 3));
        reflection->SetRepeatedUInt32(message, field, index, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_INT64: {
        int64_t val = static_cast<int64_t>(luaL_checkinteger(L, 3));
        reflection->SetRepeatedInt64(message, field, index, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_UINT64: {
        uint64_t val = static_cast<uint64_t>(luaL_checkinteger(L, 3));
        reflection->SetRepeatedUInt64(message, field, index, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_FLOAT: {
        float val = static_cast<float>(luaL_checknumber(L, 3));
        reflection->SetRepeatedFloat(message, field, index, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_DOUBLE: {
        double val = static_cast<double>(luaL_checknumber(L, 3));
        reflection->SetRepeatedDouble(message, field, index, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_STRING: {
        size_t strlen;
        const char* str = static_cast<const char*>(luaL_checklstring(L, 3, &strlen));
        reflection->SetRepeatedString(message, field, index, str);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_BYTES: {
        size_t strlen;
        const char* str = static_cast<const char*>(luaL_checklstring(L, 3, &strlen));
        reflection->SetRepeatedString(message, field, index, str);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_BOOL: {
        int val = static_cast<int>(lua_toboolean(L, 3));
        reflection->SetRepeatedBool(message, field, index, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_ENUM: {
        int val = static_cast<int>(luaL_checkinteger(L, 3));
        reflection->SetRepeatedEnumValue(message, field, index, val);
    }
    break;

    default:
        luaL_argerror(L, (2), "pb_repeated_set type for set not support!!!");
        return 0;
    }

    return 1;
}

///////////////////////////////////////////////////////////
static int pb_import(lua_State* L) {
    fprintf(stdout, "pb_import enter\n");
    const char* filename = luaL_checkstring(L, 1);

    ProtoImporter::Instance()->Import(filename);
    fprintf(stdout, "pb_import leave\n");
    return 0;
}

static int pb_new(lua_State* L) {
    const char* type_name = luaL_checkstring(L, 1);
    Message* message = ProtoImporter::Instance()->CreateMessage(type_name);

    if (!message) {
        fprintf(stderr, "pb_new error, result is typename(%s) not found!\n", type_name);
        return 0;
    }

    return push_message(L, message, true);
}

static int pb_delete(lua_State* L) {
    lua_pbmsg* luamsg = (lua_pbmsg*)luaL_checkudata(L, 1, PB_MESSAGE_META);

    if (luamsg->isDelete && luamsg->msg) {
        Message* message = luamsg->msg;
        delete message;
        luamsg->msg = NULL;
    }

    return 0;
}

static int pb_tostring(lua_State* L) {
    lua_pbmsg* luamsg = (lua_pbmsg*)luaL_checkudata(L, 1, PB_MESSAGE_META);
    google::protobuf::Message* message = luamsg->msg;

    if (!message) {
        luaL_argerror(L, 1, "pb_tostring,  pb msg is nil");
        return 0;
    }

    std::string msg(message->DebugString());
    lua_pushlstring(L, msg.c_str(), msg.length());
    return 1;
}

static int pb_get(lua_State* L) {
    lua_pbmsg* luamsg = (lua_pbmsg*)luaL_checkudata(L, 1, PB_MESSAGE_META);
    const char* field_name = luaL_checkstring(L, 2);

    Message* message = luamsg->msg;

    if (!message) {
        luaL_argerror(L, 1, "pb_get,  pb msg is nil");
        return 0;
    }

    const Descriptor* descriptor = message->GetDescriptor();
    const Reflection* reflection = message->GetReflection();
    const FieldDescriptor* field = descriptor->FindFieldByName(field_name);
    luaL_argcheck(L, (field != NULL), 2, "pb_get, field_name error");

    if (field->is_repeated()) {
        return push_repeated_msg(L, message, const_cast<FieldDescriptor*>(field));
    }

    switch (field->type()) {
    case google::protobuf::FieldDescriptor::TYPE_MESSAGE: {
        Message* msg = reflection->MutableMessage(message, field);
        return push_message(L, msg, false);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_INT32: {
        lua_pushinteger(L, reflection->GetInt32(*message, field));
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_UINT32: {
        lua_pushinteger(L, reflection->GetUInt32(*message, field));
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_INT64: {
        lua_pushinteger(L, reflection->GetInt64(*message, field));
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_UINT64: {
        lua_pushinteger(L, reflection->GetUInt64(*message, field));
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_FLOAT: {
        lua_pushnumber(L, reflection->GetFloat(*message, field));
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_DOUBLE: {
        lua_pushnumber(L, reflection->GetDouble(*message, field));
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_STRING: {
        std::string str(reflection->GetString(*message, field));
        lua_pushlstring(L, str.c_str(), str.length());
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_BYTES: {
        std::string str(reflection->GetString(*message, field));
        lua_pushlstring(L, str.c_str(), str.length());
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_BOOL: {
        lua_pushboolean(L, reflection->GetBool(*message, field));
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_ENUM: {
        lua_pushinteger(L, reflection->GetEnumValue(*message, field));
    }
    break;

    default:
        luaL_argerror(L, (2),
                      "pb_repeated_add field name type for add  is not support!!");
        return 0;
    }

    return 1;
}

static int pb_set(lua_State* L) {
    lua_pbmsg* luamsg = (lua_pbmsg*)luaL_checkudata(L, 1, PB_MESSAGE_META);
    const char* field_name = luaL_checkstring(L, 2);

    Message* message = luamsg->msg;

    if (!message) {
        luaL_argerror(L, 1, "pb_set, pb msg is nil");
        return 0;
    }

    const Descriptor* descriptor = message->GetDescriptor();
    const Reflection* reflection = message->GetReflection();
    const FieldDescriptor* field = descriptor->FindFieldByName(field_name);

    luaL_argcheck(L, field != NULL, 2, "LuaPB::set field_name error");
    luaL_argcheck(L, !field->is_repeated(), 2, "LuaPB::set field_name is repeated");

    switch (field->type()) {
    case google::protobuf::FieldDescriptor::TYPE_MESSAGE: {
        luaL_argerror(L, 2, "pb_set field_name type_message is not support!!");
        return 0;
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_INT32: {
        int32_t val = static_cast<int32_t>(luaL_checkinteger(L, 3));
        reflection->SetInt32(message, field, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_UINT32: {
        uint32_t val = static_cast<uint32_t>(luaL_checkinteger(L, 3));
        reflection->SetUInt32(message, field, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_INT64: {
        int64_t val = static_cast<int64_t>(luaL_checknumber(L, 3));
        reflection->SetInt64(message, field, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_UINT64: {
        uint64_t val = static_cast<uint64_t>(luaL_checknumber(L, 3));
        reflection->SetUInt64(message, field, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_FLOAT: {
        float val = static_cast<float>(luaL_checknumber(L, 3));
        reflection->SetFloat(message, field, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_DOUBLE: {
        double val = static_cast<double>(luaL_checknumber(L, 3));
        reflection->SetDouble(message, field, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_STRING: {
        size_t strlen;
        const char* str = luaL_checklstring(L, 3, &strlen);
        reflection->SetString(message, field, str);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_BYTES: {
        size_t strlen;
        const char* str = luaL_checklstring(L, 3, &strlen);
        reflection->SetString(message, field, str);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_BOOL: {
        int val = static_cast<int>(luaL_checkinteger(L, 3));
        reflection->SetBool(message, field, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_ENUM: {
        int val = static_cast<int>(luaL_checkinteger(L, 3));
        reflection->SetEnumValue(message, field, val);
    }
    break;

    default:
        luaL_argerror(L, 2, "pb_set field_name type error");
        return 0;
    }

    return 1;
}

static int pb_parseFromString(lua_State* L) {
    lua_pbmsg* luamsg = (lua_pbmsg*)luaL_checkudata(L, 1, PB_MESSAGE_META);
    Message* message = luamsg->msg;

    luaL_checktype(L, 2, LUA_TSTRING);

    size_t bin_len;
    const char* bin = static_cast<const char*>( luaL_checklstring(L, 2, &bin_len));
    message->ParseFromArray(bin, (int)bin_len);
    return 0;
}

static int pb_serializeToString(lua_State* L) {
    lua_pbmsg* luamsg = (lua_pbmsg*)luaL_checkudata(L, 1, PB_MESSAGE_META);
    Message* message = luamsg->msg;

    std::string msg;
    message->SerializeToString(&msg);
    lua_pushlstring(L, msg.c_str(), msg.length());
    return 1;
}

static int encode(lua_State *L){
    lua_pbmsg* luamsg = (lua_pbmsg*)luaL_checkudata(L, -1, PB_MESSAGE_META);
    Message* message = luamsg->msg;

	std::string buffer;
	if (!message->SerializeToString(&buffer)) {
		printf("Failed to serialize message: proto.%s\n", lua_tostring(L, -2));
	}

	lua_pushlstring(L, buffer.c_str(), buffer.length());

	return 1;
}

static int decode(lua_State *L) {
	const std::string type_name(lua_tostring(L, -2));
    Message* message = ProtoImporter::Instance()->CreateMessage(type_name);
    if (NULL == message)
    {
        return 0;
    }
	size_t len;
	const char *s = lua_tolstring(L, -1, &len);
	std::string buffer(s, len);
	if(!message->ParseFromString(buffer)) {
		printf("Failed to parse message: proto.%s\n", lua_tostring(L, -2));
	}

    push_message(L, message, true);

	return 1;
}

static const struct luaL_Reg lib[] = {
    {"new", pb_new},
    {"import", pb_import},
    {"tostring", pb_tostring},
    {"parseFromString", pb_parseFromString},
    {"serializeToString", pb_serializeToString},
    {"decode", decode},
	{"encode", encode},
    {NULL, NULL}
};

static const struct luaL_Reg libm[] = {
    {"__index", pb_get},
    {"__newindex", pb_set},
    {"__gc", pb_delete},
    {NULL, NULL}
};

static const struct luaL_Reg repeatedlib[] = {
    {"add", pb_repeated_add},
    {"len", pb_repeated_len},
    {"get", pb_repeated_get},
    {"set", pb_repeated_set},
    {NULL, NULL},
};

LUAMOD_API int luaopen_luapb(lua_State* L) {
    luaL_newmetatable(L, PB_REPEATED_MESSAGE_META);
    luaL_register(L, NULL, repeatedlib);

    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);

    lua_pushstring(L, "__newindex");
    lua_pushcfunction(L, pb_repeated_set);
    lua_settable(L, -3);

    luaL_newmetatable(L, PB_MESSAGE_META);
    luaL_register(L, NULL, libm);

    luaL_register(L, PB_MESSAGE, lib);
    return 1;
}

LUAMOD_API int require_luapb(lua_State* L) {
    luaL_requiref(L, "luapb", luaopen_luapb, 0);
    printf("lua module: require luapb\n");
    return 1;
}