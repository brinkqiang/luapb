﻿
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


#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor_database.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/util/json_util.h>

#include "luapb_module.h"
#include "protoimporter.h"

#include "pugixml.hpp"
#include "yaml-cpp/yaml.h"
#include "yaml-cpp/emittermanip.h"

#include "dmutil.h"

#undef GetMessage

using namespace google::protobuf;
using namespace pugi;

#ifndef DM_INDENT
#define DM_INDENT "\t"
#endif

#ifndef DM_FLAG
#define DM_FLAG (format_no_declaration)
//#define DM_FLAG (format_raw | format_no_declaration)
#endif

#ifndef DM_ENCODING
#define DM_ENCODING encoding_auto
#endif

typedef struct tagluamsg
{
    google::protobuf::Message* msg;
    bool isDelete; //是否要释放掉这个msg;
} lua_pbmsg;

typedef struct tagluarepeatedmsg
{
    google::protobuf::Message* msg;
    google::protobuf::FieldDescriptor* field;
} lua_repeated_msg;

typedef struct tagluamapmsg
{
    google::protobuf::Message* msg;
    google::protobuf::FieldDescriptor* field;
} lua_map_msg;

#define PB_MESSAGE "pb"
#define PB_MAP_MESSAGE_META "pb_map_meta"
#define PB_MESSAGE_META "pb_meta"

#define PB_REPEATED_MESSAGE_META "pb_repeated_meta"

static int push_message(lua_State* L,
    google::protobuf::Message* message,
    bool del)
{
    lua_pbmsg* tmp =
        static_cast<lua_pbmsg*>(lua_newuserdata(L, sizeof(lua_pbmsg)));

    if (tmp == NULL)
    {
        return 0;
    }

    tmp->msg = message;
    tmp->isDelete = del;

    luaL_getmetatable(L, PB_MESSAGE_META);
    lua_setmetatable(L, -2);
    return 1;
}

static int push_repeated_msg(lua_State* L,
    google::protobuf::Message* msg,
    google::protobuf::FieldDescriptor* field)
{
    lua_repeated_msg* repeated =
        static_cast<lua_repeated_msg*>(lua_newuserdata(L, sizeof(lua_repeated_msg)));

    if (!repeated)
    {
        return 0;
    }

    repeated->msg = msg;
    repeated->field = field;

    luaL_getmetatable(L, PB_REPEATED_MESSAGE_META);
    lua_setmetatable(L, -2);
    return 1;
}

static int pb_repeated_add(lua_State* L)
{
    lua_repeated_msg* repeated =
        (lua_repeated_msg*)luaL_checkudata(L, 1, PB_REPEATED_MESSAGE_META);

    google::protobuf::Message* message = repeated->msg;

    if (!message)
    {
        luaL_argerror(L, 1, "pb_repeated_add, pb msg is nil");
        return 0;
    }

    FieldDescriptor* field = repeated->field;
    const Reflection* reflection = message->GetReflection();
    luaL_argcheck(L, field != NULL, 1, "pb_repeated_add, field is null");

    switch (field->type())
    {
    case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
    {
        google::protobuf::Message* msg = reflection->AddMessage(message, field);
        return push_message(L, msg, false);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_INT32:
    {
        int32_t val = static_cast<int32_t>(luaL_checkinteger(L, 2));
        reflection->AddInt32(message, field, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_UINT32:
    {
        uint32_t val = static_cast<uint32_t>(luaL_checknumber(L, 2));
        reflection->AddUInt32(message, field, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_INT64:
    {
        int64_t val = static_cast<int64_t>(luaL_checknumber(L, 2));
        reflection->AddInt64(message, field, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_UINT64:
    {
        uint64_t val = static_cast<uint64_t>(luaL_checknumber(L, 2));
        reflection->AddUInt64(message, field, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_FLOAT:
    {
        float val = static_cast<float>(luaL_checknumber(L, 2));
        reflection->AddFloat(message, field, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
    {
        double val = static_cast<double>(luaL_checknumber(L, 2));
        reflection->AddDouble(message, field, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_STRING:
    {
        size_t strlen;
        const char* str = luaL_checklstring(L, 2, &strlen);
        reflection->AddString(message, field, str);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_BYTES:
    {
        size_t strlen;
        const char* str = luaL_checklstring(L, 2, &strlen);
        reflection->AddString(message, field, str);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_BOOL:
    {
        int val = static_cast<int>(luaL_checkinteger(L, 2));
        reflection->AddBool(message, field, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_ENUM:
    {
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

static int pb_repeated_len(lua_State* L)
{
    lua_repeated_msg* repeated =
        (lua_repeated_msg*)luaL_checkudata(L, 1, PB_REPEATED_MESSAGE_META);

    google::protobuf::Message* message = repeated->msg;

    if (!message)
    {
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

static int pb_repeated_get(lua_State* L)
{
    lua_repeated_msg* repeated =
        (lua_repeated_msg*)luaL_checkudata(L, 1, PB_REPEATED_MESSAGE_META);

    google::protobuf::Message* message = repeated->msg;

    if (!message)
    {
        luaL_argerror(L, 1, "pb_repeated_get, pb msg is nil");
        return 0;
    }

    google::protobuf::FieldDescriptor* field = repeated->field;
    const google::protobuf::Reflection* reflection = message->GetReflection();
    luaL_argcheck(L, field != NULL, 1, "pb_repeated_get field not exist");

    int index = static_cast<int>(luaL_checkinteger(L, 2)) - 1;
    luaL_argcheck(L, index >= 0, 2, "pb_repeated_get index expected >= 1");

    switch (field->type())
    {
    case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
    {
        google::protobuf::Message* msg = reflection->MutableRepeatedMessage(message, field, index);
        return push_message(L, msg, false);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_INT32:
    {
        lua_pushinteger(L, reflection->GetRepeatedInt32(*message, field, index));
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_UINT32:
    {
        lua_pushinteger(L, reflection->GetRepeatedUInt32(*message, field, index));
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_INT64:
    {
        lua_pushinteger(L, reflection->GetRepeatedInt64(*message, field, index));
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_UINT64:
    {
        lua_pushinteger(L, reflection->GetRepeatedUInt64(*message, field, index));
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_FLOAT:
    {
        lua_pushnumber(L, reflection->GetRepeatedFloat(*message, field, index));
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
    {
        lua_pushnumber(L, reflection->GetRepeatedDouble(*message, field, index));
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_STRING:
    {
        lua_pushstring(L, reflection->GetRepeatedString(*message, field, index).data());
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_BYTES:
    {
        std::string str = reflection->GetRepeatedString(*message, field, index);
        lua_pushlstring(L, str.data(), str.size());
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_BOOL:
    {
        lua_pushboolean(L, reflection->GetRepeatedBool(*message, field, index));
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_ENUM:
    {
        lua_pushinteger(L, reflection->GetRepeatedEnumValue(*message, field, index));
    }
    break;

    default:
        luaL_argerror(L, 0, "pb_repeated_get, field type for get not support!!!");
        return 0;
    }

    return 1;
}

static int pb_repeated_set(lua_State* L)
{
    lua_repeated_msg* repeated =
        (lua_repeated_msg*)luaL_checkudata(L, 1, PB_REPEATED_MESSAGE_META);

    google::protobuf::Message* message = repeated->msg;

    if (!message)
    {
        luaL_argerror(L, 1, "pb_repeated_set, pb msg is nil");
        return 0;
    }

    const google::protobuf::Reflection* reflection = message->GetReflection();
    google::protobuf::FieldDescriptor* field = repeated->field;
    luaL_argcheck(L, field != NULL, 1, "pb_repeated_set field not exist");

    int index = static_cast<int>(luaL_checkinteger(L, 2)) - 1;
    luaL_argcheck(L, index >= 0, 2, "pb_repeated_set index expected >= 1");

    switch (field->type())
    {
    case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
    {
        luaL_argerror(L, (2), "pb_repeated_set type_message for set not support!!!");
        return 0;
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_INT32:
    {
        int32_t val = static_cast<int32_t>(luaL_checkinteger(L, 3));
        reflection->SetRepeatedInt32(message, field, index, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_UINT32:
    {
        uint32_t val = static_cast<uint32_t>(luaL_checkinteger(L, 3));
        reflection->SetRepeatedUInt32(message, field, index, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_INT64:
    {
        int64_t val = static_cast<int64_t>(luaL_checkinteger(L, 3));
        reflection->SetRepeatedInt64(message, field, index, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_UINT64:
    {
        uint64_t val = static_cast<uint64_t>(luaL_checkinteger(L, 3));
        reflection->SetRepeatedUInt64(message, field, index, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_FLOAT:
    {
        float val = static_cast<float>(luaL_checknumber(L, 3));
        reflection->SetRepeatedFloat(message, field, index, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
    {
        double val = static_cast<double>(luaL_checknumber(L, 3));
        reflection->SetRepeatedDouble(message, field, index, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_STRING:
    {
        size_t strlen;
        const char* str = static_cast<const char*>(luaL_checklstring(L, 3, &strlen));
        reflection->SetRepeatedString(message, field, index, str);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_BYTES:
    {
        size_t strlen;
        const char* str = static_cast<const char*>(luaL_checklstring(L, 3, &strlen));
        reflection->SetRepeatedString(message, field, index, str);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_BOOL:
    {
        int val = static_cast<int>(lua_toboolean(L, 3));
        reflection->SetRepeatedBool(message, field, index, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_ENUM:
    {
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
static int pb_import(lua_State* L)
{
    fprintf(stdout, "pb_import enter\n");
    const char* filename = luaL_checkstring(L, 1);

    ProtoImporterMgr::Instance()->Import(filename);
    fprintf(stdout, "pb_import leave\n");
    return 0;
}

static int pb_new(lua_State* L)
{
    const char* type_name = luaL_checkstring(L, 1);
    google::protobuf::Message* message = ProtoImporterMgr::Instance()->CreateMessage(type_name);

    if (!message)
    {
        fprintf(stderr, "pb_new error, result is typename(%s) not found!\n", type_name);
        return 0;
    }

    return push_message(L, message, true);
}

static int pb_delete(lua_State* L)
{
    lua_pbmsg* luamsg = (lua_pbmsg*)luaL_checkudata(L, 1, PB_MESSAGE_META);

    if (luamsg->isDelete && luamsg->msg)
    {
        google::protobuf::Message* message = luamsg->msg;
        delete message;
        luamsg->msg = NULL;
    }

    return 0;
}

static int pb_tostring(lua_State* L)
{
    lua_pbmsg* luamsg = (lua_pbmsg*)luaL_checkudata(L, 1, PB_MESSAGE_META);
    google::protobuf::Message* message = luamsg->msg;

    if (!message)
    {
        luaL_argerror(L, 1, "pb_tostring,  pb msg is nil");
        return 0;
    }

    std::string msg(message->DebugString());
    lua_pushlstring(L, msg.c_str(), msg.length());
    return 1;
}

static int pb_map_add(lua_State* L)
{
    lua_map_msg* map = (lua_map_msg*)luaL_checkudata(L, 1, PB_MAP_MESSAGE_META);
    google::protobuf::Message* message = map->msg;

    if (!message)
    {
        luaL_argerror(L, 1, "pb_map_add: pb msg is nil");
        return 0;
    }

    const FieldDescriptor* field = map->field;
    const Reflection* reflection = message->GetReflection();
    luaL_argcheck(L, field != nullptr, 1, "pb_map_add: field is null");

    // 确保字段类型为Message且为Map类型
    if (field->cpp_type() != FieldDescriptor::CPPTYPE_MESSAGE || !field->is_map())
    {
        luaL_argerror(L, 1, "pb_map_add: field must be a map message type");
        return 0;
    }

    // 获取Map条目的原型
    const Message* prototype = reflection->GetMessageFactory()->GetPrototype(field->message_type());
    if (!prototype)
    {
        luaL_argerror(L, 1, "pb_map_add: failed to get map entry prototype");
        return 0;
    }

    // 添加新条目到Map并推入Lua栈
    Message* new_entry = reflection->AddMessage(message, field, reflection->GetMessageFactory());
    return push_message(L, new_entry, false);
}

static int pb_map_get(lua_State* L)
{
    lua_map_msg* map = (lua_map_msg*)luaL_checkudata(L, 1, PB_MAP_MESSAGE_META);
    google::protobuf::Message* message = map->msg;

    if (!message)
    {
        luaL_argerror(L, 1, "pb_map_get, pb msg is nil");
        return 0;
    }

    const Reflection* reflection = message->GetReflection();
    FieldDescriptor* field = map->field;
    luaL_argcheck(L, field != NULL, 1, "pb_map_get field not exist");

    const char* key = luaL_checkstring(L, 2);
    // Map字段实际是repeated条目
    const RepeatedPtrField<Message>& entries = reflection->GetRepeatedPtrField<Message>(*message, field);
    const Descriptor* entry_desc = field->message_type();
    const FieldDescriptor* key_field = entry_desc->FindFieldByName("key");
    const FieldDescriptor* value_field = entry_desc->FindFieldByName("value");

    if (!key_field || !value_field || !field->is_repeated())
    {
        luaL_argerror(L, 1, "pb_map_get, map message must have key and value fields");
        return 0;
    }

    // 遍历查找匹配key的条目
    for (const Message& entry : entries) {
        if (entry.GetReflection()->GetString(entry, key_field) == key) {
            return push_message(L, const_cast<Message*>(&entry), false);
        }
    }
    
    // 未找到返回nil
    lua_pushnil(L);
    return 1;
}

static int pb_map_set(lua_State* L)
{
    lua_map_msg* map = (lua_map_msg*)luaL_checkudata(L, 1, PB_MAP_MESSAGE_META);
    google::protobuf::Message* message = map->msg;

    if (!message)
    {
        luaL_argerror(L, 1, "pb_map_set, pb msg is nil");
        return 0;
    }

    const Reflection* reflection = message->GetReflection();
    FieldDescriptor* field = map->field;
    luaL_argcheck(L, field != NULL, 1, "pb_map_set field not exist");

    const char* key = luaL_checkstring(L, 2);
    Message* msg = reflection->MutableMessage(message, field);
    const FieldDescriptor* key_field = msg->GetDescriptor()->FindFieldByName("key");
    const FieldDescriptor* value_field = msg->GetDescriptor()->FindFieldByName("value");

    if (!key_field || !value_field)
    {
        luaL_argerror(L, 1, "pb_map_set, map message must have key and value fields");
        return 0;
    }

    reflection->SetString(msg, key_field, key);
    return 0;
}

static int pb_get(lua_State* L)
{
    lua_pbmsg* luamsg = (lua_pbmsg*)luaL_checkudata(L, 1, PB_MESSAGE_META);
    const char* field_name = luaL_checkstring(L, 2);

    google::protobuf::Message* message = luamsg->msg;

    if (!message)
    {
        luaL_argerror(L, 1, "pb_get,  pb msg is nil");
        return 0;
    }

    const google::protobuf::Descriptor* descriptor = message->GetDescriptor();
    const google::protobuf::Reflection* reflection = message->GetReflection();
    const google::protobuf::FieldDescriptor* field = descriptor->FindFieldByName(field_name);
    luaL_argcheck(L, (field != NULL), 2, "pb_get, field_name error");

    if (field->is_map())
    {
        lua_map_msg* map = (lua_map_msg*)lua_newuserdata(L, sizeof(lua_map_msg));
        map->msg = message;
        map->field = const_cast<FieldDescriptor*>(field);
        luaL_getmetatable(L, PB_MAP_MESSAGE_META);
        lua_setmetatable(L, -2);
        return 1;
    }
    else if (field->is_repeated())
    {
        return push_repeated_msg(L, message, const_cast<FieldDescriptor*>(field));
    }

    switch (field->type())
    {
    case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
    {
        google::protobuf::Message* msg = reflection->MutableMessage(message, field);
        return push_message(L, msg, false);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_INT32:
    {
        lua_pushinteger(L, reflection->GetInt32(*message, field));
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_UINT32:
    {
        lua_pushinteger(L, reflection->GetUInt32(*message, field));
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_INT64:
    {
        lua_pushinteger(L, reflection->GetInt64(*message, field));
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_UINT64:
    {
        lua_pushinteger(L, reflection->GetUInt64(*message, field));
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_FLOAT:
    {
        lua_pushnumber(L, reflection->GetFloat(*message, field));
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
    {
        lua_pushnumber(L, reflection->GetDouble(*message, field));
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_STRING:
    {
        std::string str(reflection->GetString(*message, field));
        lua_pushlstring(L, str.c_str(), str.length());
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_BYTES:
    {
        std::string str(reflection->GetString(*message, field));
        lua_pushlstring(L, str.c_str(), str.length());
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_BOOL:
    {
        lua_pushboolean(L, reflection->GetBool(*message, field));
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_ENUM:
    {
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

static int pb_set(lua_State* L)
{
    lua_pbmsg* luamsg = (lua_pbmsg*)luaL_checkudata(L, 1, PB_MESSAGE_META);
    const char* field_name = luaL_checkstring(L, 2);

    google::protobuf::Message* message = luamsg->msg;

    if (!message)
    {
        luaL_argerror(L, 1, "pb_set, pb msg is nil");
        return 0;
    }

    const google::protobuf::Descriptor* descriptor = message->GetDescriptor();
    const google::protobuf::Reflection* reflection = message->GetReflection();
    const google::protobuf::FieldDescriptor* field = descriptor->FindFieldByName(field_name);

    luaL_argcheck(L, field != NULL, 2, "LuaPB::set field_name error");
    luaL_argcheck(L, !field->is_repeated(), 2, "LuaPB::set field_name is repeated");

    switch (field->type())
    {
    case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
    {
        luaL_argerror(L, 2, "pb_set field_name type_message is not support!!");
        return 0;
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_INT32:
    {
        int32_t val = static_cast<int32_t>(luaL_checkinteger(L, 3));
        reflection->SetInt32(message, field, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_UINT32:
    {
        uint32_t val = static_cast<uint32_t>(luaL_checkinteger(L, 3));
        reflection->SetUInt32(message, field, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_INT64:
    {
        int64_t val = static_cast<int64_t>(luaL_checkinteger(L, 3));
        reflection->SetInt64(message, field, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_UINT64:
    {
        uint64_t val = static_cast<uint64_t>(luaL_checkinteger(L, 3));
        reflection->SetUInt64(message, field, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_FLOAT:
    {
        float val = static_cast<float>(luaL_checknumber(L, 3));
        reflection->SetFloat(message, field, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
    {
        double val = static_cast<double>(luaL_checknumber(L, 3));
        reflection->SetDouble(message, field, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_STRING:
    {
        size_t strlen;
        const char* str = luaL_checklstring(L, 3, &strlen);
        reflection->SetString(message, field, str);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_BYTES:
    {
        size_t strlen;
        const char* str = luaL_checklstring(L, 3, &strlen);
        reflection->SetString(message, field, str);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_BOOL:
    {
        int val = static_cast<int>(luaL_checkinteger(L, 3));
        reflection->SetBool(message, field, val);
    }
    break;

    case google::protobuf::FieldDescriptor::TYPE_ENUM:
    {
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

static int pb_parseFromString(lua_State* L)
{
    lua_pbmsg* luamsg = (lua_pbmsg*)luaL_checkudata(L, 1, PB_MESSAGE_META);
    google::protobuf::Message* message = luamsg->msg;

    luaL_checktype(L, 2, LUA_TSTRING);

    size_t bin_len;
    const char* bin = static_cast<const char*>(luaL_checklstring(L, 2, &bin_len));
    message->ParseFromArray(bin, (int)bin_len);
    return 0;
}

static int pb_serializeToString(lua_State* L)
{
    lua_pbmsg* luamsg = (lua_pbmsg*)luaL_checkudata(L, 1, PB_MESSAGE_META);
    google::protobuf::Message* message = luamsg->msg;

    std::string msg;
    message->SerializeToString(&msg);
    lua_pushlstring(L, msg.c_str(), msg.length());
    return 1;
}

/*******************************************************/
/*  parse a message from a lua table                   */
/*******************************************************/
#define CASE_FIELD_TYPE(cpptype, protobuf_method, lua_method)       \
    case FieldDescriptor::CPPTYPE_##cpptype:                        \
    {                                                               \
        reflection->protobuf_method(msg, field, lua_method(L, -1)); \
        break;                                                      \
    }

#define SWITCH_FIELD_TYPE(method_prefix, arg1, arg2)                                      \
    switch (field->cpp_type())                                                            \
    {                                                                                     \
        CASE_FIELD_TYPE(INT32, method_prefix##Int32, lua_tointeger)                       \
        CASE_FIELD_TYPE(INT64, method_prefix##Int64, lua_tointeger)                       \
        CASE_FIELD_TYPE(UINT32, method_prefix##UInt32, lua_tointeger)                     \
        CASE_FIELD_TYPE(UINT64, method_prefix##UInt64, lua_tointeger)                     \
        CASE_FIELD_TYPE(FLOAT, method_prefix##Float, lua_tonumber)                        \
        CASE_FIELD_TYPE(DOUBLE, method_prefix##Double, lua_tonumber)                      \
        CASE_FIELD_TYPE(BOOL, method_prefix##Bool, lua_toboolean)                         \
        CASE_FIELD_TYPE(STRING, method_prefix##String, lua_tostring)                      \
    case FieldDescriptor::CPPTYPE_ENUM:                                                   \
    {                                                                                     \
        const std::string name(lua_tostring(L, -1));                                      \
        const EnumValueDescriptor *valueDesc = field->enum_type()->FindValueByName(name); \
        reflection->method_prefix##Enum(msg, field, valueDesc);                           \
        break;                                                                            \
    }                                                                                     \
    case FieldDescriptor::CPPTYPE_MESSAGE:                                                \
    {                                                                                     \
        const std::string &name = field->message_type()->full_name();                     \
        Message *submsg = ProtoImporterMgr::Instance()->CreateMessage(name);              \
        ParseMessage(L, submsg);                                                          \
        reflection->method_prefix##AllocatedMessage(msg, arg1, arg2);                     \
        break;                                                                            \
    }                                                                                     \
    default:                                                                              \
    {                                                                                     \
        printf("Unknown cpptype!\n");                                                     \
        ProtoImporterMgr::Instance()->ReleaseMessage(msg);                                \
        break;                                                                            \
    }                                                                                     \
    }

// have to set the message table at top of lua_state* L
static void ParseMessage(lua_State* L, Message* msg)
{

    const Descriptor* descriptor = msg->GetDescriptor();
    const Reflection* reflection = msg->GetReflection();
    for (int i = 0; i < descriptor->field_count(); i++)
    {
        const FieldDescriptor* field = descriptor->field(i);
        const std::string& name = field->name();

        lua_getfield(L, -1, name.c_str());

        if (lua_isnil(L, -1) && field->is_required())
        {
            printf("Error: a required field in message is missing!\n");
            ProtoImporterMgr::Instance()->ReleaseMessage(msg);
            return;
        }

        if (!lua_isnil(L, -1))
        {
            if (field->is_repeated())
            {
                lua_pushnil(L);
                while (lua_next(L, -2) != 0)
                {
                    SWITCH_FIELD_TYPE(Add, field, submsg)
                        lua_pop(L, 1);
                }
            }
            else
            {
                SWITCH_FIELD_TYPE(Set, submsg, field)
            }
        }

        lua_pop(L, 1);
    }
}

#undef SWITCH_FIELD_TYPE
#undef CASE_FIELD_TYPE
#undef WriteMessage
#undef GetMessage
/*******************************************************/
/*  write a protobuf class object into a lua table     */
/*******************************************************/
#define CASE_FIELD_TYPE(cpptype, lua_method, protobuf_method, ...)                 \
    case FieldDescriptor::CPPTYPE_##cpptype:                                       \
        lua_method(L, reflection->protobuf_method((*pmsg), field, ##__VA_ARGS__)); \
        break;

#define SWITCH_FIELD_TYPE(prefix, ...)                                                              \
    switch (field->cpp_type())                                                                      \
    {                                                                                               \
        CASE_FIELD_TYPE(INT32, lua_pushinteger, prefix##Int32, ##__VA_ARGS__)                       \
        CASE_FIELD_TYPE(INT64, lua_pushinteger, prefix##Int64, ##__VA_ARGS__)                       \
        CASE_FIELD_TYPE(UINT32, lua_pushinteger, prefix##UInt32, ##__VA_ARGS__)                     \
        CASE_FIELD_TYPE(UINT64, lua_pushinteger, prefix##UInt64, ##__VA_ARGS__)                     \
        CASE_FIELD_TYPE(FLOAT, lua_pushnumber, prefix##Float, ##__VA_ARGS__)                        \
        CASE_FIELD_TYPE(DOUBLE, lua_pushnumber, prefix##Double, ##__VA_ARGS__)                      \
        CASE_FIELD_TYPE(BOOL, lua_pushboolean, prefix##Bool, ##__VA_ARGS__)                         \
    case FieldDescriptor::CPPTYPE_ENUM:                                                             \
        lua_pushstring(L, reflection->prefix##Enum((*pmsg), field, ##__VA_ARGS__)->name().c_str()); \
        break;                                                                                      \
    case FieldDescriptor::CPPTYPE_STRING:                                                           \
        lua_pushstring(L, reflection->prefix##String((*pmsg), field, ##__VA_ARGS__).c_str());       \
        break;                                                                                      \
    case FieldDescriptor::CPPTYPE_MESSAGE:                                                          \
        WriteMessage(L, &reflection->prefix##Message((*pmsg), field, ##__VA_ARGS__));               \
        break;                                                                                      \
    default:                                                                                        \
        printf("Unknown field!\n");                                                                 \
        break;                                                                                      \
    }

static void WriteMessage(lua_State* L, const Message* pmsg)
{
    const Descriptor* descriptor = pmsg->GetDescriptor();
    const Reflection* reflection = pmsg->GetReflection();

    lua_createtable(L, 0, descriptor->field_count());
    for (int i = 0; i < descriptor->field_count(); i++)
    {
        const FieldDescriptor* field = descriptor->field(i);
        const std::string& name = field->name();

        lua_pushstring(L, name.c_str());

        bool flag = true;
        if (field->is_repeated())
        {
            lua_createtable(L, reflection->FieldSize((*pmsg), field), 0);
            for (int j = 0; j < reflection->FieldSize((*pmsg), field); j++)
            {
                lua_pushinteger(L, j + 1);
                SWITCH_FIELD_TYPE(GetRepeated, j)
                    lua_settable(L, -3);
            }
            if (reflection->FieldSize((*pmsg), field) == 0)
            {
                lua_pop(L, 1);
                flag = false;
            }
        }
        else
        {
            //if (reflection->HasField((*pmsg), field)) {
            SWITCH_FIELD_TYPE(Get)
                //} else {
                //	flag = false;
                //}
        }
        if (flag)
        {
            lua_settable(L, -3);
        }
        else
        {
            lua_pop(L, 1);
        }
    }
}

#undef SWITCH_FIELD_TYPE
#undef CASE_FIELD_TYPE

/*****************************************************************************************/
/*  lua interface for encoding a lua table message into a binary protobuf string buffer  */
/*****************************************************************************************/
static int encode(lua_State* L)
{
    const std::string name(lua_tostring(L, -2));
    google::protobuf::Message* message = ProtoImporterMgr::Instance()->CreateMessage(name);
    if (NULL == message)
    {
        luaL_argerror(L, (2), "encode CreateMessage failed!!");
        return 0;
    }
    ParseMessage(L, message);

    std::string buffer;
    if (!message->SerializeToString(&buffer))
    {
        luaL_argerror(L, (2), "encode SerializeToString failed!!");
        return 0;
    }
    lua_pushlstring(L, buffer.c_str(), buffer.length());

    ProtoImporterMgr::Instance()->ReleaseMessage(message);
    return 1;
}

/*****************************************************************************************/
/*  lua interface for decoding a binary protobuf string buffer into a lua table message  */
/*****************************************************************************************/
static int decode(lua_State* L)
{
    const std::string name(lua_tostring(L, -2));
    google::protobuf::Message* message = ProtoImporterMgr::Instance()->CreateMessage(name);
    if (NULL == message)
    {
        luaL_argerror(L, (2), "decode CreateMessage failed!!");
        return 0;
    }
    size_t len;
    const char* s = lua_tolstring(L, -1, &len);
    std::string buffer(s, len);
    if (!message->ParseFromString(buffer))
    {
        luaL_argerror(L, (2), "decode ParseFromString failed!!");
        return 0;
    }

    WriteMessage(L, message);
    ProtoImporterMgr::Instance()->ReleaseMessage(message);
    return 1;
}

static int table2json(lua_State* L)
{
    const std::string name(lua_tostring(L, -2));
    google::protobuf::Message* message = ProtoImporterMgr::Instance()->CreateMessage(name);
    if (NULL == message)
    {
        luaL_argerror(L, (2), "table2json CreateMessage failed !!");
        return 0;
    }

    ParseMessage(L, message);

    std::string strJson;

    google::protobuf::util::JsonPrintOptions options;
    options.add_whitespace = true;
    options.always_print_primitive_fields = true;
    options.preserve_proto_field_names = true;

    google::protobuf::util::MessageToJsonString(*message, &strJson, options);
    lua_pushlstring(L, strJson.c_str(), strJson.length());

    ProtoImporterMgr::Instance()->ReleaseMessage(message);
    return 1;
}

static int json2table(lua_State* L)
{
    const std::string name(lua_tostring(L, -2));
    google::protobuf::Message* message = ProtoImporterMgr::Instance()->CreateMessage(name);
    if (NULL == message)
    {
        luaL_argerror(L, (2), "json2table CreateMessage failed !!");
        return 0;
    }
    size_t len;
    const char* s = lua_tolstring(L, -1, &len);
    std::string buffer(s, len);

    google::protobuf::util::JsonStringToMessage(buffer, message);
    WriteMessage(L, message);
    ProtoImporterMgr::Instance()->ReleaseMessage(message);
    return 1;
}

static void WriteXml(lua_State* L, pugi::xml_node& doc, google::protobuf::Message* message, const char* name)
{
    const google::protobuf::Descriptor* descriptor = message->GetDescriptor();
    const google::protobuf::Reflection* reflection = message->GetReflection();
    pugi::xml_node msg;
    if (name)
    {
        msg = doc.append_child(name);
    }
    else
    {
        //msg = doc.append_child(message->GetTypeName().c_str());
        msg = doc;
    }

    for (int i = 0; i < descriptor->field_count(); i++)
    {
        const google::protobuf::FieldDescriptor* field = descriptor->field(i);
        int index = i;

        bool flag = true;
        if (field->is_repeated())
        {
            if (field->has_default_value())
            {
                continue;
            }

            for (int j = 0; j < reflection->FieldSize((*message), field); j++)
            {
                switch (field->type())
                {
                case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
                {
                    google::protobuf::Message* _msg = reflection->MutableRepeatedMessage(message, field, j);
                    WriteXml(L, msg, _msg, field->name().c_str());
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_INT32:
                {
                    pugi::xml_node xml_field = msg.append_child(field->name().c_str());
                    xml_field.append_child(node_pcdata).set_value(reflection->GetRepeatedInt32(*message, field, j));
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_UINT32:
                {
                    pugi::xml_node xml_field = msg.append_child(field->name().c_str());
                    xml_field.append_child(node_pcdata).set_value(reflection->GetRepeatedUInt32(*message, field, j));
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_INT64:
                {
                    pugi::xml_node xml_field = msg.append_child(field->name().c_str());
                    xml_field.append_child(node_pcdata).set_value((long long)reflection->GetRepeatedInt64(*message, field, j));
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_UINT64:
                {
                    pugi::xml_node xml_field = msg.append_child(field->name().c_str());
                    xml_field.append_child(node_pcdata).set_value((unsigned long long)reflection->GetRepeatedUInt64(*message, field, j));
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_FLOAT:
                {
                    pugi::xml_node xml_field = msg.append_child(field->name().c_str());
                    xml_field.append_child(node_pcdata).set_value(reflection->GetRepeatedFloat(*message, field, j));
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
                {
                    pugi::xml_node xml_field = msg.append_child(field->name().c_str());
                    xml_field.append_child(node_pcdata).set_value(reflection->GetRepeatedDouble(*message, field, j));
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_STRING:
                {
                    pugi::xml_node xml_field = msg.append_child(field->name().c_str());
                    xml_field.append_child(node_pcdata).set_value(reflection->GetRepeatedString(*message, field, j));
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_BYTES:
                {
                    pugi::xml_node xml_field = msg.append_child(field->name().c_str());
                    xml_field.append_child(node_pcdata).set_value(reflection->GetRepeatedString(*message, field, j));
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_BOOL:
                {
                    pugi::xml_node xml_field = msg.append_child(field->name().c_str());
                    xml_field.append_child(node_pcdata).set_value(reflection->GetRepeatedBool(*message, field, j));
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_ENUM:
                {
                    pugi::xml_node xml_field = msg.append_child(field->name().c_str());
                    xml_field.append_child(node_pcdata).set_value(reflection->GetRepeatedEnumValue(*message, field, j));
                }
                break;

                default:
                    luaL_argerror(L, 0, "writexml, field type for get not support!!!");
                    return;
                }
            }

            if (reflection->FieldSize((*message), field) == 0)
            {
                flag = false;
            }
        }
        else
        {
            if (field->has_default_value())
            {
                continue;
            }

            switch (field->type())
            {
            case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
            {
                google::protobuf::Message* _msg = reflection->MutableMessage(message, field);
                WriteXml(L, msg, _msg, field->name().c_str());
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_INT32:
            {
                pugi::xml_node xml_field = msg.append_child(field->name().c_str());
                xml_field.append_child(node_pcdata).set_value(reflection->GetInt32(*message, field));
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_UINT32:
            {
                pugi::xml_node xml_field = msg.append_child(field->name().c_str());
                xml_field.append_child(node_pcdata).set_value(reflection->GetUInt32(*message, field));
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_INT64:
            {
                pugi::xml_node xml_field = msg.append_child(field->name().c_str());
                xml_field.append_child(node_pcdata).set_value((long long)reflection->GetInt64(*message, field));
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_UINT64:
            {
                pugi::xml_node xml_field = msg.append_child(field->name().c_str());
                xml_field.append_child(node_pcdata).set_value((unsigned long long)reflection->GetUInt64(*message, field));
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_FLOAT:
            {
                pugi::xml_node xml_field = msg.append_child(field->name().c_str());
                xml_field.append_child(node_pcdata).set_value(reflection->GetFloat(*message, field));
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
            {
                pugi::xml_node xml_field = msg.append_child(field->name().c_str());
                xml_field.append_child(node_pcdata).set_value(reflection->GetDouble(*message, field));
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_STRING:
            {
                pugi::xml_node xml_field = msg.append_child(field->name().c_str());
                xml_field.append_child(node_pcdata).set_value(reflection->GetString(*message, field));
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_BYTES:
            {
                pugi::xml_node xml_field = msg.append_child(field->name().c_str());
                xml_field.append_child(node_pcdata).set_value(reflection->GetString(*message, field));
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_BOOL:
            {
                pugi::xml_node xml_field = msg.append_child(field->name().c_str());
                xml_field.append_child(node_pcdata).set_value(reflection->GetBool(*message, field));
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_ENUM:
            {
                pugi::xml_node xml_field = msg.append_child(field->name().c_str());
                xml_field.append_child(node_pcdata).set_value(reflection->GetEnumValue(*message, field));
            }
            break;

            default:
                luaL_argerror(L, 0, "writexml, field type for get not support!!!");
                return;
            }
        }
    }
}

static void ReadXml(lua_State* L, pugi::xml_node& doc, google::protobuf::Message* message, const char* name = NULL)
{
    const google::protobuf::Descriptor* descriptor = message->GetDescriptor();
    const google::protobuf::Reflection* reflection = message->GetReflection();
    pugi::xml_node msg;
    if (name)
    {
        msg = doc;
    }
    else
    {
        //msg = doc.child(message->GetTypeName().c_str());
        msg = doc;
    }

    for (int i = 0; i < descriptor->field_count(); i++)
    {
        const google::protobuf::FieldDescriptor* field = descriptor->field(i);

        int index = i;

        bool flag = true;
        if (field->is_repeated())
        {
            if (field->has_default_value())
            {
                continue;
            }

            pugi::xml_object_range<pugi::xml_named_node_iterator> range = msg.children(field->name().c_str());
            xml_named_node_iterator It = range.begin();
            int i = 0;
            for (i = 0; It != range.end(); ++i, ++It)
            {
                switch (field->type())
                {
                case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
                {
                    pugi::xml_node xml_field = *It;
                    google::protobuf::Message* _msg = reflection->AddMessage(message, field);
                    ReadXml(L, xml_field, _msg, field->name().c_str());
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_INT32:
                {
                    pugi::xml_node xml_field = *It;

                    reflection->AddInt32(message, field, pugi::to_number<int>(xml_field.child_value()));
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_UINT32:
                {
                    pugi::xml_node xml_field = *It;

                    reflection->AddUInt32(message, field, pugi::to_number<unsigned int>(xml_field.child_value()));
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_INT64:
                {
                    pugi::xml_node xml_field = *It;
                    reflection->AddInt64(message, field, pugi::to_number<long long>(xml_field.child_value()));
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_UINT64:
                {
                    pugi::xml_node xml_field = *It;
                    reflection->AddUInt64(message, field, pugi::to_number<unsigned long long>(xml_field.child_value()));
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_FLOAT:
                {
                    pugi::xml_node xml_field = *It;
                    reflection->AddFloat(message, field, pugi::to_number<float>(xml_field.child_value()));
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
                {
                    pugi::xml_node xml_field = *It;
                    reflection->AddDouble(message, field, pugi::to_number<double>(xml_field.child_value()));
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_STRING:
                {
                    pugi::xml_node xml_field = *It;
                    reflection->AddString(message, field, xml_field.child_value());
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_BYTES:
                {
                    pugi::xml_node xml_field = *It;
                    reflection->AddString(message, field, xml_field.child_value());
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_BOOL:
                {
                    pugi::xml_node xml_field = *It;
                    reflection->AddBool(message, field, pugi::to_number<bool>(xml_field.child_value()));
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_ENUM:
                {
                    pugi::xml_node xml_field = *It;
                    reflection->AddEnumValue(message, field, pugi::to_number<int>(xml_field.child_value()));
                }
                break;

                default:
                    luaL_argerror(L, 0, "readxml, field type for get not support!!!");
                    return;
                }
            }
        }
        else
        {
            if (field->has_default_value())
            {
                continue;
            }

            switch (field->type())
            {
            case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
            {
                google::protobuf::Message* _msg = reflection->AddMessage(message, field);
                ReadXml(L, msg, _msg);
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_INT32:
            {
                pugi::xml_node xml_field = msg.child(field->name().c_str());

                reflection->SetInt32(message, field, pugi::to_number<int>(xml_field.child_value()));
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_UINT32:
            {
                pugi::xml_node xml_field = msg.child(field->name().c_str());

                reflection->SetUInt32(message, field, pugi::to_number<unsigned int>(xml_field.child_value()));
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_INT64:
            {
                pugi::xml_node xml_field = msg.child(field->name().c_str());
                reflection->SetInt64(message, field, pugi::to_number<long long>(xml_field.child_value()));
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_UINT64:
            {
                pugi::xml_node xml_field = msg.child(field->name().c_str());
                reflection->SetUInt64(message, field, pugi::to_number<unsigned long long>(xml_field.child_value()));
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_FLOAT:
            {
                pugi::xml_node xml_field = msg.child(field->name().c_str());
                reflection->SetFloat(message, field, pugi::to_number<float>(xml_field.child_value()));
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
            {
                pugi::xml_node xml_field = msg.child(field->name().c_str());
                reflection->SetDouble(message, field, pugi::to_number<double>(xml_field.child_value()));
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_STRING:
            {
                pugi::xml_node xml_field = msg.child(field->name().c_str());
                reflection->SetString(message, field, xml_field.child_value());
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_BYTES:
            {
                pugi::xml_node xml_field = msg.child(field->name().c_str());
                reflection->SetString(message, field, xml_field.child_value());
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_BOOL:
            {
                pugi::xml_node xml_field = msg.child(field->name().c_str());
                reflection->SetBool(message, field, pugi::to_number<bool>(xml_field.child_value()));
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_ENUM:
            {
                pugi::xml_node xml_field = msg.child(field->name().c_str());
                reflection->SetEnumValue(message, field, pugi::to_number<int>(xml_field.child_value()));
            }
            break;

            default:
                luaL_argerror(L, 0, "readxml, field type for get not support!!!");
                return;
            }
        }
    }
}

static int table2xml(lua_State* L)
{
    const std::string name(lua_tostring(L, -2));
    google::protobuf::Message* message = ProtoImporterMgr::Instance()->CreateMessage(name);
    if (NULL == message)
    {
        luaL_argerror(L, (2), "table2xml CreateMessage failed !!");
        return 0;
    }

    ParseMessage(L, message);

    pugi::xml_document doc;
    WriteXml(L, doc, message, NULL);

    pugi::xml_string_writer writer;
    doc.save(writer, DM_INDENT, DM_FLAG, DM_ENCODING);

    lua_pushlstring(L, writer.result.c_str(), writer.result.length());

    ProtoImporterMgr::Instance()->ReleaseMessage(message);
    return 1;
}

static int xml2table(lua_State* L)
{
    const std::string name(lua_tostring(L, -2));
    google::protobuf::Message* message = ProtoImporterMgr::Instance()->CreateMessage(name);
    if (NULL == message)
    {
        luaL_argerror(L, (2), "xml2table CreateMessage failed !!");
        return 0;
    }
    size_t len;
    const char* s = lua_tolstring(L, -1, &len);
    std::string buffer(s, len);

    pugi::xml_document doc;
    doc.load_buffer(buffer.c_str(), buffer.size());

    ReadXml(L, doc, message);
    WriteMessage(L, message);

    ProtoImporterMgr::Instance()->ReleaseMessage(message);
    return 1;
}

static void WriteYaml(lua_State* L, YAML::Node& doc, google::protobuf::Message* message, const char* name)
{
    const google::protobuf::Descriptor* descriptor = message->GetDescriptor();
    const google::protobuf::Reflection* reflection = message->GetReflection();
    YAML::Node msg;
    if (name)
    {
        doc[name].push_back(msg);
    }
    else
    {
        //msg = doc.append_child(message->GetTypeName().c_str());
        msg = doc;
    }

    for (int i = 0; i < descriptor->field_count(); i++)
    {
        const google::protobuf::FieldDescriptor* field = descriptor->field(i);
        int index = i;

        bool flag = true;
        if (field->is_repeated())
        {
            if (field->has_default_value())
            {
                continue;
            }

            for (int j = 0; j < reflection->FieldSize((*message), field); j++)
            {
                switch (field->type())
                {
                case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
                {
                    google::protobuf::Message* _msg = reflection->MutableRepeatedMessage(message, field, j);
                    WriteYaml(L, msg, _msg, field->name().c_str());
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_INT32:
                {
                    YAML::Node xml_field = msg[field->name()];
                    xml_field.push_back(reflection->GetRepeatedUInt32(*message, field, j));
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_UINT32:
                {
                    YAML::Node xml_field = msg[field->name()];
                    xml_field.push_back(reflection->GetRepeatedUInt32(*message, field, j));
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_INT64:
                {
                    YAML::Node xml_field = msg[field->name()];
                    xml_field.push_back(reflection->GetRepeatedInt64(*message, field, j));
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_UINT64:
                {
                    YAML::Node xml_field = msg[field->name()];
                    xml_field.push_back(reflection->GetRepeatedUInt64(*message, field, j));
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_FLOAT:
                {
                    YAML::Node xml_field = msg[field->name()];
                    xml_field.push_back(reflection->GetRepeatedFloat(*message, field, j));
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
                {
                    YAML::Node xml_field = msg[field->name()];
                    xml_field.push_back(reflection->GetRepeatedDouble(*message, field, j));
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_STRING:
                {
                    YAML::Node xml_field = msg[field->name()];
                    xml_field.push_back(reflection->GetRepeatedString(*message, field, j));
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_BYTES:
                {
                    YAML::Node xml_field = msg[field->name()];
                    xml_field.push_back(reflection->GetRepeatedString(*message, field, j));
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_BOOL:
                {
                    YAML::Node xml_field = msg[field->name()];
                    xml_field.push_back(reflection->GetRepeatedBool(*message, field, j));
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_ENUM:
                {
                    YAML::Node xml_field = msg[field->name()];
                    xml_field.push_back(reflection->GetRepeatedEnumValue(*message, field, j));
                }
                break;

                default:
                    luaL_argerror(L, 0, "writeyaml, field type for get not support!!!");
                    return;
                }
            }

            if (reflection->FieldSize((*message), field) == 0)
            {
                flag = false;
            }
        }
        else
        {
            if (field->has_default_value())
            {
                continue;
            }

            switch (field->type())
            {
            case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
            {
                google::protobuf::Message* _msg = reflection->MutableMessage(message, field);
                WriteYaml(L, msg, _msg, field->name().c_str());
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_INT32:
            {
                YAML::Node xml_field = msg[field->name()];
                xml_field = reflection->GetInt32(*message, field);
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_UINT32:
            {
                YAML::Node xml_field = msg[field->name()];
                xml_field = reflection->GetUInt32(*message, field);
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_INT64:
            {
                YAML::Node xml_field = msg[field->name()];
                xml_field = reflection->GetInt64(*message, field);
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_UINT64:
            {
                YAML::Node xml_field = msg[field->name()];
                xml_field = reflection->GetUInt64(*message, field);
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_FLOAT:
            {
                YAML::Node xml_field = msg[field->name()];
                xml_field = reflection->GetFloat(*message, field);
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
            {
                YAML::Node xml_field = msg[field->name()];
                xml_field = reflection->GetDouble(*message, field);
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_STRING:
            {
                YAML::Node xml_field = msg[field->name()];
                xml_field = reflection->GetString(*message, field);
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_BYTES:
            {
                YAML::Node xml_field = msg[field->name()];
                xml_field = reflection->GetString(*message, field);
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_BOOL:
            {
                YAML::Node xml_field = msg[field->name()];
                xml_field = reflection->GetBool(*message, field);
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_ENUM:
            {
                YAML::Node xml_field = msg[field->name()];
                xml_field = reflection->GetEnumValue(*message, field);
            }
            break;

            default:
                luaL_argerror(L, 0, "writeyaml, field type for get not support!!!");
                return;
            }
        }
    }
}

static void ReadYaml(lua_State* L, YAML::Node& doc, google::protobuf::Message* message, const char* name = NULL)
{
    const google::protobuf::Descriptor* descriptor = message->GetDescriptor();
    const google::protobuf::Reflection* reflection = message->GetReflection();
    YAML::Node msg;
    if (name)
    {
        msg = doc;
    }
    else
    {
        //msg = doc.child(message->GetTypeName().c_str());
        msg = doc;
    }

    for (int i = 0; i < descriptor->field_count(); i++)
    {
        const google::protobuf::FieldDescriptor* field = descriptor->field(i);

        int index = i;

        bool flag = true;
        if (field->is_repeated())
        {
            if (field->has_default_value())
            {
                continue;
            }

            YAML::Node range = msg[field->name()];
            YAML::iterator It = range.begin();
            int i = 0;
            for (i = 0; It != range.end(); ++i, ++It)
            {
                switch (field->type())
                {
                case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
                {
                    YAML::Node xml_field = *It;
                    google::protobuf::Message* _msg = reflection->AddMessage(message, field);
                    ReadYaml(L, xml_field, _msg, field->name().c_str());
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_INT32:
                {
                    YAML::Node xml_field = *It;
                    reflection->AddInt32(message, field, xml_field.as<int>());
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_UINT32:
                {
                    YAML::Node xml_field = *It;
                    reflection->AddUInt32(message, field, xml_field.as<unsigned int>());
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_INT64:
                {
                    YAML::Node xml_field = *It;
                    reflection->AddInt64(message, field, xml_field.as<long long>());
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_UINT64:
                {
                    YAML::Node xml_field = *It;
                    reflection->AddUInt64(message, field, xml_field.as<unsigned long long>());
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_FLOAT:
                {
                    YAML::Node xml_field = *It;
                    reflection->AddFloat(message, field, xml_field.as<float>());
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
                {
                    YAML::Node xml_field = *It;
                    reflection->AddDouble(message, field, xml_field.as<double>());
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_STRING:
                {
                    YAML::Node xml_field = *It;
                    reflection->AddString(message, field, xml_field.as<std::string>());
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_BYTES:
                {
                    YAML::Node xml_field = *It;
                    reflection->AddString(message, field, xml_field.as<std::string>());
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_BOOL:
                {
                    YAML::Node xml_field = *It;
                    reflection->AddBool(message, field, xml_field.as<bool>());
                }
                break;

                case google::protobuf::FieldDescriptor::TYPE_ENUM:
                {
                    YAML::Node xml_field = *It;
                    reflection->AddEnumValue(message, field, xml_field.as<int>());
                }
                break;

                default:
                    luaL_argerror(L, 0, "readyaml, field type for get not support!!!");
                    return;
                }
            }
        }
        else
        {
            if (field->has_default_value())
            {
                continue;
            }

            switch (field->type())
            {
            case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
            {
                google::protobuf::Message* _msg = reflection->AddMessage(message, field);
                ReadYaml(L, msg, _msg);
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_INT32:
            {
                YAML::Node xml_field = msg[field->name()];
                reflection->SetInt32(message, field, xml_field.as<int>());
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_UINT32:
            {
                YAML::Node xml_field = msg[field->name()];
                reflection->SetUInt32(message, field, xml_field.as<unsigned int>());
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_INT64:
            {
                YAML::Node xml_field = msg[field->name()];
                reflection->SetInt64(message, field, xml_field.as<long long>());
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_UINT64:
            {
                YAML::Node xml_field = msg[field->name()];
                reflection->SetUInt64(message, field, xml_field.as<unsigned long long>());
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_FLOAT:
            {
                YAML::Node xml_field = msg[field->name()];
                reflection->SetFloat(message, field, xml_field.as<float>());
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
            {
                YAML::Node xml_field = msg[field->name()];
                reflection->SetDouble(message, field, xml_field.as<double>());
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_STRING:
            {
                YAML::Node xml_field = msg[field->name()];
                reflection->SetString(message, field, xml_field.as<std::string>());
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_BYTES:
            {
                YAML::Node xml_field = msg[field->name()];
                reflection->SetString(message, field, xml_field.as<std::string>());
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_BOOL:
            {
                YAML::Node xml_field = msg[field->name()];
                reflection->SetBool(message, field, xml_field.as<bool>());
            }
            break;

            case google::protobuf::FieldDescriptor::TYPE_ENUM:
            {
                YAML::Node xml_field = msg[field->name()];
                reflection->SetEnumValue(message, field, xml_field.as<int>());
            }
            break;
            default:
                luaL_argerror(L, 0, "readxml, field type for get not support!!!");
                return;
            }
        }
    }
}

static int table2yaml(lua_State* L)
{
    const std::string name(lua_tostring(L, -2));
    google::protobuf::Message* message = ProtoImporterMgr::Instance()->CreateMessage(name);
    if (NULL == message)
    {
        luaL_argerror(L, (2), "table2yaml CreateMessage failed !!");
        return 0;
    }

    ParseMessage(L, message);

    YAML::Node doc;
    WriteYaml(L, doc, message, NULL);

    std::stringstream ss;
    ss << doc;
    std::string strDoc = ss.str();

    lua_pushlstring(L, strDoc.c_str(), strDoc.length());

    ProtoImporterMgr::Instance()->ReleaseMessage(message);

    return 1;
}

static int yaml2table(lua_State* L)
{
    const std::string name(lua_tostring(L, -2));
    google::protobuf::Message* message = ProtoImporterMgr::Instance()->CreateMessage(name);
    if (NULL == message)
    {
        luaL_argerror(L, (2), "yaml2table CreateMessage failed !!");
        return 0;
    }
    size_t len;
    const char* s = lua_tolstring(L, -1, &len);
    std::string buffer(s, len);

    YAML::Node doc = YAML::Load(buffer.c_str());
    ReadYaml(L, doc, message);
    WriteMessage(L, message);

    ProtoImporterMgr::Instance()->ReleaseMessage(message);
    return 1;
}

static int xml2json(lua_State* L)
{
    const std::string name(lua_tostring(L, -2));
    google::protobuf::Message* message = ProtoImporterMgr::Instance()->CreateMessage(name);
    if (NULL == message)
    {
        luaL_argerror(L, (2), "xml2json CreateMessage failed !!");
        return 0;
    }
    size_t len;
    const char* s = lua_tolstring(L, -1, &len);
    std::string buffer(s, len);

    pugi::xml_document doc;
    doc.load_buffer(buffer.c_str(), buffer.size());

    ReadXml(L, doc, message);
    WriteMessage(L, message);

    std::string strJson;

    google::protobuf::util::JsonPrintOptions options;
    options.add_whitespace = true;
    options.always_print_primitive_fields = true;
    options.preserve_proto_field_names = true;

    google::protobuf::util::MessageToJsonString(*message, &strJson, options);
    lua_pushlstring(L, strJson.c_str(), strJson.length());

    ProtoImporterMgr::Instance()->ReleaseMessage(message);
    return 1;
}

static int json2xml(lua_State* L)
{
    const std::string name(lua_tostring(L, -2));
    google::protobuf::Message* message = ProtoImporterMgr::Instance()->CreateMessage(name);
    if (NULL == message)
    {
        luaL_argerror(L, (2), "json2xml CreateMessage failed !!");
        return 0;
    }
    size_t len;
    const char* s = lua_tolstring(L, -1, &len);
    std::string buffer(s, len);
    google::protobuf::util::JsonStringToMessage(buffer, message);
    ProtoImporterMgr::Instance()->ReleaseMessage(message);
    return 1;
}

static int id2name(lua_State* L)
{
    int msgID(lua_tointeger(L, -1));
    std::string strName = ProtoImporterMgr::Instance()->ID2Name(msgID);

    lua_pushlstring(L, strName.c_str(), strName.length());
    return 1;
}

static int name2id(lua_State* L)
{
    const std::string name(lua_tostring(L, -1));
    int msgID = ProtoImporterMgr::Instance()->Name2ID(name);
    lua_pushinteger(L, (lua_Integer)msgID);
    return 1;
}

static int getmodulepath(lua_State* L)
{
    std::string strPath = DMGetModulePath();
    lua_pushlstring(L, strPath.c_str(), strPath.length());
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
    {"id2name", id2name},
    {"name2id", name2id},
    {"table2json", table2json},
    {"json2table", json2table},
    {"table2xml", table2xml},
    {"xml2table", xml2table},
    {"xml2json", xml2json},
    {"json2xml", json2xml},
    {"table2yaml", table2yaml},
    {"yaml2table", yaml2table},
    {"getmodulepath", getmodulepath},
    {NULL, NULL}
};

static const struct luaL_Reg libm[] = {
    {"__index", pb_get},
    {"__newindex", pb_set},
    {"__gc", pb_delete},
    {NULL, NULL} };

static const struct luaL_Reg repeatedlib[] = {
    {"add", pb_repeated_add},
    {"len", pb_repeated_len},
    {"get", pb_repeated_get},
    {"set", pb_repeated_set},
    {NULL, NULL},
};

static const struct luaL_Reg maplib[] = {
    {"add", pb_map_add},
    {"get", pb_map_get},
    {"set", pb_map_set},
    {NULL, NULL},
};

LUAMOD_API int luaopen_luapb(lua_State* L)
{
    luaL_newmetatable(L, PB_REPEATED_MESSAGE_META);
    luaL_register(L, NULL, repeatedlib);

    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);

    lua_pushstring(L, "__newindex");
    lua_pushcfunction(L, pb_repeated_set);
    lua_settable(L, -3);

    luaL_newmetatable(L, PB_MAP_MESSAGE_META);
    luaL_register(L, NULL, maplib);

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

LUAMOD_API int require_luapb(lua_State* L)
{
    luaL_requiref(L, "luapb", luaopen_luapb, 0);
    printf("lua module: require luapb\n");
    return 1;
}
