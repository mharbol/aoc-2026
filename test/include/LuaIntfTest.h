
#ifndef __LUA_TEST_H__
#define __LUA_TEST_H__

#include "LuaIntf.h"
#include <gtest/gtest.h>

#include <string>

class LuaIntfTest : public ::testing::Test {
protected:
    lua::LuaIntf intf{};

    void LUA_DO_STRING(const std::string &cmd) {
        ASSERT_TRUE(intf.luaDoString(cmd)) << "Failed to run: " << cmd;
    };
};

#endif
