
#include <cstdint>
#include <gtest/gtest.h>
#include <tuple>

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
    ASSERT_TRUE(a.has_value()) << "Expect 'a' should be a global value: " << a.error();
    ASSERT_EQ(4, a.value());
}

TEST_F(LuaIntfTest, TestCallSimpleFunc) {
    LUA_DO_STRING("function retVal(a) return a end");
    auto ret = intf.call<uint32_t>("retVal", 10);
    ASSERT_TRUE(ret.has_value()) << "Expect ret should be a value: " << ret.error();
    ASSERT_EQ(10, ret.value());
}

TEST_F(LuaIntfTest, TestCallAddFunc) {
    LUA_DO_STRING("function retVal(a, b) return a + b end");
    auto ret = intf.call<uint32_t>("retVal", 5, 5);
    ASSERT_TRUE(ret.has_value()) << "Expect ret should be a value: " << ret.error();
    ASSERT_EQ(10, ret.value());
}

TEST_F(LuaIntfTest, TestCallBiggerFunction) {
    LUA_DO_STRING("function add(a, b) return a + b end");
    LUA_DO_STRING("function retVal(a, b) return add(a, b) end");
    auto ret = intf.call<uint32_t>("retVal", 5, 5);
    ASSERT_TRUE(ret.has_value()) << "Expect ret should be a value: " << ret.error();
    ASSERT_EQ(10, ret.value());
}

TEST_F(LuaIntfTest, TestCallDivFunction) {
    LUA_DO_STRING("function retVal(a, b) return a // b end");
    auto ret = intf.call<uint32_t>("retVal", 12, 6);
    ASSERT_TRUE(ret.has_value()) << "Expect ret should be a value: " << ret.error();
    ASSERT_EQ(2, ret.value());
}

TEST_F(LuaIntfTest, TestCallNoArgsFunction) {
    LUA_DO_STRING("function retVal() return 10 end");
    auto ret = intf.call<uint32_t>("retVal");
    ASSERT_TRUE(ret.has_value()) << "Expect ret should be a value: " << ret.error();
    ASSERT_EQ(10, ret.value());
}

TEST_F(LuaIntfTest, TestCallStringFunction) {
    LUA_DO_STRING("function retVal(a) return #a end");
    auto ret = intf.call<uint32_t>("retVal", "0123456789");
    ASSERT_TRUE(ret.has_value()) << "Expect ret should be a value: " << ret.error();
    ASSERT_EQ(10, ret.value());
}

TEST_F(LuaIntfTest, TestCallMixedFunction) {
    LUA_DO_STRING("function retVal(str, num) return #str + num end");
    auto ret = intf.call<uint32_t>("retVal", "0123456789", 10);
    ASSERT_TRUE(ret.has_value()) << "Expect ret should be a value: " << ret.error();
    ASSERT_EQ(20, ret.value());
}

TEST_F(LuaIntfTest, TestCallStringRetFunction) {
    LUA_DO_STRING("function retVal(s1, s2) return s1 .. ' ' .. s2 end");
    auto ret = intf.call<std::string>("retVal", "hello", "world");
    ASSERT_TRUE(ret.has_value()) << "Expect ret should be a value: " << ret.error();
    ASSERT_EQ("hello world", ret.value());
}

TEST_F(LuaIntfTest, TestLuaGetStack) {
    intf.pushStack("hello"); // -4
    intf.pushStack(1.0);     // -3
    intf.pushStack(false);   // -2
    intf.pushStack(10);      // -1

    auto ret1 = intf.luaGetStack<int>(-1);
    ASSERT_TRUE(ret1.has_value()) << "Expect ret1 should be a value: " << ret1.error();
    ASSERT_EQ(10, ret1.value());

    auto ret2 = intf.luaGetStack<bool>(-2);
    ASSERT_TRUE(ret2.has_value()) << "Expect ret2 should be a value: " << ret2.error();
    ASSERT_EQ(false, ret2.value());

    auto ret3 = intf.luaGetStack<float>(-3);
    ASSERT_TRUE(ret3.has_value()) << "Expect ret3 should be a value: " << ret3.error();
    ASSERT_EQ(1.0, ret3.value());

    auto ret4 = intf.luaGetStack<std::string>(-4);
    ASSERT_TRUE(ret4.has_value()) << "Expect ret4 should be a value: " << ret4.error();
    ASSERT_EQ("hello", ret4.value());
}

TEST_F(LuaIntfTest, TestLuaPopStack) {
    intf.pushStack(10); // -1
    auto ret1 = intf.popStack<uint32_t>();
    ASSERT_TRUE(ret1.has_value()) << "Expect ret1 should be a value: " << ret1.error();
    ASSERT_EQ(10, ret1.value());
}

TEST_F(LuaIntfTest, TestLuaPopMultiStack) {
    intf.pushStack(1.2);  // -3
    intf.pushStack(true); // -2
    intf.pushStack(10);   // -1
    auto ret1 = intf.popMultiStack<uint32_t, bool, float>();
    ASSERT_TRUE(ret1.has_value()) << "Expect ret1 should be a value: " << ret1.error();
    constexpr std::tuple<uint32_t, bool, float> theReturn{10, true, 1.2};
    ASSERT_EQ(theReturn, ret1.value());
}
