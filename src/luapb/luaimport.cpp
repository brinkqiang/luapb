/*
** Lua binding: luaimport
** Generated automatically by tolua++-1.0.93.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua++.h"

/* Exported function */
TOLUA_API int  tolua_luaimport_open (lua_State* tolua_S);

#include "dmlua_typeid.h"
#include "protoimporter.h"
#include "google/protobuf/dynamic_message.h"
#include "google/protobuf/compiler/importer.h"

/* function to release collected object via destructor */
#ifdef __cplusplus

static int tolua_collect_ProtoImporterImpl (lua_State* tolua_S)
{
 ProtoImporterImpl* self = (ProtoImporterImpl*) tolua_tousertype(tolua_S,1,0);
	Mtolua_delete(self);
	return 0;
}
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"ProtoImporterImpl");
 Mtolua_typeid< ProtoImporterImpl >(tolua_S, "ProtoImporterImpl");
 tolua_usertype(tolua_S,"google::protobuf::Message");
 Mtolua_typeid< google::protobuf::Message >(tolua_S, "google::protobuf::Message");
}

/* method: new of class  ProtoImporterImpl */
#ifndef TOLUA_DISABLE_tolua_luaimport_ProtoImporterImpl_new00
static int tolua_luaimport_ProtoImporterImpl_new00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"ProtoImporterImpl",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   ProtoImporterImpl* tolua_ret = (ProtoImporterImpl*)  Mtolua_new((ProtoImporterImpl)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"ProtoImporterImpl");
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: new_local of class  ProtoImporterImpl */
#ifndef TOLUA_DISABLE_tolua_luaimport_ProtoImporterImpl_new00_local
static int tolua_luaimport_ProtoImporterImpl_new00_local(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertable(tolua_S,1,"ProtoImporterImpl",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  {
   ProtoImporterImpl* tolua_ret = (ProtoImporterImpl*)  Mtolua_new((ProtoImporterImpl)());
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"ProtoImporterImpl");
    tolua_register_gc(tolua_S,lua_gettop(tolua_S));
  }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'new'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: delete of class  ProtoImporterImpl */
#ifndef TOLUA_DISABLE_tolua_luaimport_ProtoImporterImpl_delete00
static int tolua_luaimport_ProtoImporterImpl_delete00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ProtoImporterImpl",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ProtoImporterImpl* self = (ProtoImporterImpl*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'delete'", NULL);
#endif
  Mtolua_delete(self);
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'delete'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: Import of class  ProtoImporterImpl */
#ifndef TOLUA_DISABLE_tolua_luaimport_ProtoImporterImpl_Import00
static int tolua_luaimport_ProtoImporterImpl_Import00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ProtoImporterImpl",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ProtoImporterImpl* self = (ProtoImporterImpl*)  tolua_tousertype(tolua_S,1,0);
  const std::string strFileName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Import'", NULL);
#endif
  {
   bool tolua_ret = (bool)  self->Import(strFileName);
   tolua_pushboolean(tolua_S,(bool)tolua_ret);
   tolua_pushcppstring(tolua_S,(const char*)strFileName);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Import'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: CreateMessage of class  ProtoImporterImpl */
#ifndef TOLUA_DISABLE_tolua_luaimport_ProtoImporterImpl_CreateMessage00
static int tolua_luaimport_ProtoImporterImpl_CreateMessage00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ProtoImporterImpl",0,&tolua_err) ||
     !tolua_iscppstring(tolua_S,2,0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ProtoImporterImpl* self = (ProtoImporterImpl*)  tolua_tousertype(tolua_S,1,0);
  const std::string strTypeName = ((const std::string)  tolua_tocppstring(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'CreateMessage'", NULL);
#endif
  {
   google::protobuf::Message* tolua_ret = (google::protobuf::Message*)  self->CreateMessage(strTypeName);
    tolua_pushusertype(tolua_S,(void*)tolua_ret,"google::protobuf::Message");
   tolua_pushcppstring(tolua_S,(const char*)strTypeName);
  }
 }
 return 2;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'CreateMessage'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* method: ReleaseMessage of class  ProtoImporterImpl */
#ifndef TOLUA_DISABLE_tolua_luaimport_ProtoImporterImpl_ReleaseMessage00
static int tolua_luaimport_ProtoImporterImpl_ReleaseMessage00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
     !tolua_isusertype(tolua_S,1,"ProtoImporterImpl",0,&tolua_err) ||
     !tolua_isusertype(tolua_S,2,"google::protobuf::Message",0,&tolua_err) ||
     !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
  goto tolua_lerror;
 else
#endif
 {
  ProtoImporterImpl* self = (ProtoImporterImpl*)  tolua_tousertype(tolua_S,1,0);
  google::protobuf::Message* message = ((google::protobuf::Message*)  tolua_tousertype(tolua_S,2,0));
#ifndef TOLUA_RELEASE
  if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ReleaseMessage'", NULL);
#endif
  {
   self->ReleaseMessage(message);
  }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ReleaseMessage'.",&tolua_err);
 return 0;
#endif
}
#endif //#ifndef TOLUA_DISABLE

/* Open function */
TOLUA_API int tolua_luaimport_open (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
  #ifdef __cplusplus
  tolua_cclass(tolua_S,"ProtoImporterImpl","ProtoImporterImpl","",tolua_collect_ProtoImporterImpl);
  #else
  tolua_cclass(tolua_S,"ProtoImporterImpl","ProtoImporterImpl","",NULL);
  #endif
  tolua_beginmodule(tolua_S,"ProtoImporterImpl");
   tolua_function(tolua_S,"new",tolua_luaimport_ProtoImporterImpl_new00);
   tolua_function(tolua_S,"new_local",tolua_luaimport_ProtoImporterImpl_new00_local);
   tolua_function(tolua_S,".call",tolua_luaimport_ProtoImporterImpl_new00_local);
   tolua_function(tolua_S,"delete",tolua_luaimport_ProtoImporterImpl_delete00);
   tolua_function(tolua_S,"Import",tolua_luaimport_ProtoImporterImpl_Import00);
   tolua_function(tolua_S,"CreateMessage",tolua_luaimport_ProtoImporterImpl_CreateMessage00);
   tolua_function(tolua_S,"ReleaseMessage",tolua_luaimport_ProtoImporterImpl_ReleaseMessage00);
  tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 TOLUA_API int luaopen_luaimport (lua_State* tolua_S) {
 return tolua_luaimport_open(tolua_S);
};
#endif

