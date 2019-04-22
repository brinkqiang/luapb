#include "test/role/rolemgr.h"
#include "test/role/role.h"
#include "dmlua.h"
#include "gtest/gtest.h"

#define DMLUA_TEST_COUNT 10000

//TEST(luabasetest, luabasetest) {
//    CDMLuaEngine oDMLuaEngine;
//
//    if (!oDMLuaEngine.ReloadScript()) {
//        ASSERT_TRUE(0);
//        return;
//    }
//
//    oDMLuaEngine.DoString(
//        "function addtest()\n"
//        "   local a = 100000000\n"
//        "   local b = 100000000\n"
//        "   local c = a * b\n"
//        "   print(c)\n"
//        "end\n");
//    {
//        for (int i = 0; i < 1; ++i) {
//            int r = oDMLuaEngine.Call("addtest");
//        }
//    }
//
//    if (!oDMLuaEngine.ReloadScript()) {
//        ASSERT_TRUE(0);
//        return;
//    }
//
//    oDMLuaEngine.DoString(
//        "function addex(first, second)\n"
//        "   return first * second\n"
//        "end\n");
//    {
//        for (int i = 0; i < 1; ++i) {
//            uint64_t r = oDMLuaEngine.CallT<uint64_t>("addex", 100000000LL, 100000000LL);
//
//            if (r >= 0) {
//                std::cout << r << std::endl;
//            }
//        }
//    }
//    {
//        oDMLuaEngine.DoString(
//            "function script.task.taskinfo()\n"
//            "   local task = STaskInfo:new()\n"
//            "   task.nTaskID = 1002\n"
//            "   task.nTaskState = 2\n"
//            "   task.nTaskCondition = 2\n"
//            "   return task\n"
//            "end\n");
//
//        STaskInfo sInfo = oDMLuaEngine.CallT<STaskInfo>("script.task.taskinfo");
//
//        std::cout << sInfo.nTaskID << std::endl;
//    }
//    {
//        oDMLuaEngine.DoString(
//            "function script.task.taskinfo(task)\n"
//            "   task.nTaskID = 1003\n"
//            "   task.nTaskState = 1\n"
//            "   task.nTaskCondition = 0\n"
//            "end\n");
//        STaskInfo sInfo;
//        int r = oDMLuaEngine.Call("script.task.taskinfo", &sInfo);
//
//        if (r >= 0) {
//            std::cout << sInfo.nTaskID << std::endl;
//        }
//    }
//    {
//        oDMLuaEngine.DoString(
//            "function bintest(data)\n"
//            "   print(string.len(data))"
//            "   print(data)"
//            "end\n");
//        std::string strData = "12345";
//        strData.append("\0", 1);
//        strData.append("ABCDE", 5);
//        int r = oDMLuaEngine.Call("bintest", strData);
//        if (r >= 0) {
//            ;
//        }
//    }
//
//    CRole* poRole = CRoleMgr::Instance()->CreateRole();
//    poRole->SetName("andy");
//    poRole->SetHp(9999);
//    poRole->SetMp(9999);
//    unsigned int dwTaskID = 100;
//
//    LResultINT oResult(-1);
//    oDMLuaEngine.Call("script.task.task.AcceptTask", poRole, dwTaskID, &oResult);
//
//    oDMLuaEngine.Call("script.task.task.FinishTask", poRole, dwTaskID);
//    std::vector<std::string> vecData;
//    vecData.push_back("hello");
//    oDMLuaEngine.Call("script.common.test.main", &vecData);
//    oDMLuaEngine.Call("script.config.loadcsv.main");
//    CRoleMgr::Instance()->ReleaseRole(poRole);
//}
//
//TEST(luaload, luaload) {
//    if (!CDMLuaEngine::Instance()->ReloadScript()) {
//        ASSERT_TRUE(0);
//        return;
//    }
//
//    CDMLuaEngine::Instance()->DoString(
//        "function Sqrt(x\n)"
//        "MAX_LOOP = 100000000\n"
//        "z = 1.0\n"
//        "for i = 1, MAX_LOOP do\n"
//        "z = z - (z*z - x) / (2 * z)\n"
//        "end\n"
//        "return z\n"
//        "end\n");
//}
//
//TEST(lua_empty_call, lua_empty_call) {
//    CDMLuaEngine oDMLuaEngine;
//    if (!oDMLuaEngine.ReloadScript()) {
//        ASSERT_TRUE(0);
//        return;
//    }
//    oDMLuaEngine.DoString(
//        "function performancetest()\n"
//        "end\n");
//
//    for (int i = 0; i < DMLUA_TEST_COUNT; ++i) {
//        oDMLuaEngine.Call("performancetest");
//    }
//}
//TEST(lua_tb_create, lua_tb_create) {
//    CDMLuaEngine oDMLuaEngine;
//    if (!oDMLuaEngine.ReloadScript()) {
//        ASSERT_TRUE(0);
//        return;
//    }
//
//    for (int i = 0; i < DMLUA_TEST_COUNT; ++i) {
//        oDMLuaEngine.Call("script.msg.msg.tbcreate");
//    }
//}
//
//TEST(lua_pbcodec, lua_pbcodec) {
//    CDMLuaEngine oDMLuaEngine;
//    if (!oDMLuaEngine.ReloadScript()) {
//        ASSERT_TRUE(0);
//        return;
//    }
//
//    for (int i = 0; i < DMLUA_TEST_COUNT; ++i) {
//        oDMLuaEngine.Call("script.msg.msg.pbtest");
//    }
//}

TEST(lua_profiler, lua_profiler) {
    CDMLuaEngine oDMLuaEngine;
    if (!oDMLuaEngine.ReloadScript()) {
        ASSERT_TRUE(0);
        return;
    }

    oDMLuaEngine.Call("script.debug.profiler.profiler_start");
    oDMLuaEngine.Call("script.msg.msg.pbtest");
    oDMLuaEngine.Call("script.msg.msg.tbcreate");
    oDMLuaEngine.Call("script.debug.profiler.profiler_stop");
}