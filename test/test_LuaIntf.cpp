
#include <gtest/gtest.h>

#include "LuaIntf.h"
#include "LuaIntfTest.h"

TEST_F(LuaIntfTest, TestPushPopStack) {
    intf.pushStack(5);
    intf.pushStack(1.5);
    intf.pushStack(false);
    intf.pushStack("hey");
    std::string s = "hello";
    intf.pushStack(s);
    intf.pushStack(nullptr);
}

TEST_F(LuaIntfTest, TestBasicDoString) {
    LUA_DO_STRING("a = 2 + 2");
    const auto a = intf.getGlobal<uint32_t>("a");
    ASSERT_TRUE(a.has_value()) << "Expect 'a' should be a global value";
    ASSERT_EQ(4, a.value());
}

TEST_F(LuaIntfTest, TestCallSimpleFunc) {
    LUA_DO_STRING("function retVal(a) return a end");
    auto ret = intf.call<uint32_t>("retVal", 10);
    ASSERT_TRUE(ret.has_value()) << "Expect ret should be a value";
    ASSERT_EQ(10, ret.value());
}

TEST_F(LuaIntfTest, TestCallAddFunc) {
    LUA_DO_STRING("function retVal(a, b) return a + b end");
    auto ret = intf.call<uint32_t>("retVal", 5, 5);
    ASSERT_TRUE(ret.has_value()) << "Expect ret should be a value";
    ASSERT_EQ(10, ret.value());
}

TEST_F(LuaIntfTest, TestCallBiggerFunction) {
    LUA_DO_STRING("function add(a, b) return a + b end");
    LUA_DO_STRING("function retVal(a, b) return add(a, b) end");
    auto ret = intf.call<uint32_t>("retVal", 5, 5);
    ASSERT_TRUE(ret.has_value()) << "Expect ret should be a value";
    ASSERT_EQ(10, ret.value());
}

TEST_F(LuaIntfTest, TestCallDivFunction) {
    LUA_DO_STRING("function retVal(a, b) return a / b end");
    auto ret = intf.call<uint32_t>("retVal", 12, 6);
    ASSERT_TRUE(ret.has_value()) << "Expect ret should be a value";
    ASSERT_EQ(2, ret.value());
}

TEST_F(LuaIntfTest, TestCallNoArgsFunction) {
    LUA_DO_STRING("function retVal() return 10 end");
    auto ret = intf.call<uint32_t>("retVal");
    ASSERT_TRUE(ret.has_value()) << "Expect ret should be a value";
    ASSERT_EQ(10, ret.value());
}
