
#ifndef _LUA_TEST_H_
#define _LUA_TEST_H_

#include "LuaIntf.h"
#include <gtest/gtest.h>

#include <string>

/**
 * @brief Test a new `LuaIntf`.
 */
class LuaIntfTest : public ::testing::Test {
protected:
    lua::LuaIntf intf{};

    void LUA_DO_STRING(const std::string &cmd) {
        ASSERT_TRUE(intf.luaDoString(cmd)) << "Failed to run: " << cmd;
    }
};

/**
 * @brief Test a new `LuaIntf` and ensure the starting and ending sizes are the same when
 * testing.
 */
class LuaIntfStackTest : public LuaIntfTest {
protected:
    /// @brief Initial size of stack to be checked against at the end of a test
    size_t init_lua_stack_sz{};

    void SetUp() override {
        init_lua_stack_sz = intf.getStackSize();
    }

    void TearDown() override {
        EXPECT_EQ(init_lua_stack_sz, intf.getStackSize())
            << "Expected initial and final stack sizes to be the same";
    }
};

#endif
