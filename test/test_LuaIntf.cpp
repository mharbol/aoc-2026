
#include <cstdint>
#include <gtest/gtest.h>
#include <optional>
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

TEST_F(LuaIntfTest, TestCallVoidFunction) {
    LUA_DO_STRING("function retVal() end");
    auto ret = intf.call<void>("retVal");
    ASSERT_TRUE(ret.has_value()) << "Expect ret should be a value: " << ret.error();

    // call the 'addToA' function twice
    LUA_DO_STRING("a = 0");
    LUA_DO_STRING("function addToA(x) a = a + x end");
    ret = intf.call<void>("addToA", 1);
    ASSERT_TRUE(ret.has_value()) << "Expect ret should be a value: " << ret.error();
    auto a = intf.getGlobal<uint32_t>("a");
    ASSERT_TRUE(a.has_value()) << "Expect 'a' should be a value: " << a.error();
    ASSERT_EQ(a, 1);
    ret = intf.call<void>("addToA", 2);
    ASSERT_TRUE(ret.has_value()) << "Expect ret should be a value: " << ret.error();
    a = intf.getGlobal<uint32_t>("a");
    ASSERT_TRUE(a.has_value()) << "Expect 'a' should be a value: " << a.error();
    ASSERT_EQ(a, 3);
}

TEST_F(LuaIntfTest, TestCallStringRetFunction) {
    LUA_DO_STRING("function retVal(s1, s2) return s1 .. ' ' .. s2 end");
    auto ret = intf.call<std::string>("retVal", "hello", "world");
    ASSERT_TRUE(ret.has_value()) << "Expect ret should be a value: " << ret.error();
    ASSERT_EQ("hello world", ret.value());
}

TEST_F(LuaIntfTest, TestLuaGetStack) {
    intf.pushStack(nullptr); // -5
    intf.pushStack("hello"); // -4
    intf.pushStack(1.0);     // -3
    intf.pushStack(false);   // -2
    intf.pushStack(10);      // -1

    auto ret1 = intf.luaGetStack<int>(-1);
    ASSERT_TRUE(ret1.has_value()) << "Expect ret1 should be a value: " << ret1.error();
    ASSERT_EQ(10, ret1.value());

    auto ret1_optional = intf.luaGetStack<std::optional<int>>(-1);
    ASSERT_TRUE(ret1_optional.has_value())
        << "Expect ret1_option should be a value: " << ret1_optional.error();
    ASSERT_EQ(10, ret1_optional.value());

    auto ret2 = intf.luaGetStack<bool>(-2);
    ASSERT_TRUE(ret2.has_value()) << "Expect ret2 should be a value: " << ret2.error();
    ASSERT_EQ(false, ret2.value());

    auto ret3 = intf.luaGetStack<float>(-3);
    ASSERT_TRUE(ret3.has_value()) << "Expect ret3 should be a value: " << ret3.error();
    ASSERT_EQ(1.0, ret3.value());

    auto ret4 = intf.luaGetStack<std::string>(-4);
    ASSERT_TRUE(ret4.has_value()) << "Expect ret4 should be a value: " << ret4.error();
    ASSERT_EQ("hello", ret4.value());

    auto ret5 = intf.luaGetStack<std::optional<std::string>>(-5);
    ASSERT_TRUE(ret5.has_value()) << "Expect ret5 should be a value: " << ret5.error();
    ASSERT_EQ(std::nullopt, ret5.value());
}

TEST_F(LuaIntfTest, TestLuaPopStack) {
    intf.pushStack(10); // -1
    auto ret1 = intf.popStack<uint32_t>();
    ASSERT_TRUE(ret1.has_value()) << "Expect ret1 should be a value: " << ret1.error();
    ASSERT_EQ(10, ret1.value());
}

TEST_F(LuaIntfTest, TestLuaPopStackVoid) {

    const size_t init_stack_size = intf.getStackSize();

    intf.pushStack(10); // -1
    auto ret1 = intf.popStack<void>();
    ASSERT_TRUE(ret1.has_value()) << "Expect ret1 should be a value: " << ret1.error();

    ASSERT_EQ(init_stack_size + 1, intf.getStackSize())
        << "Stack should be one larger than initial";

    auto ret2 = intf.popStack<uint32_t>();
    ASSERT_TRUE(ret2.has_value()) << "Expect ret2 should be a value: " << ret1.error();
    ASSERT_EQ(10, ret2.value());

    ASSERT_EQ(init_stack_size, intf.getStackSize()) << "Stack should be same size as initial";
}

TEST_F(LuaIntfTest, TestLuaPopMultiStack) {
    intf.pushStack(1.2);  // -3
    intf.pushStack(true); // -2
    intf.pushStack(10);   // -1
    auto ret1 = intf.popMultiStack<uint32_t, bool, float>();
    ASSERT_TRUE(ret1.has_value()) << "Expect ret1 should be a value: " << ret1.error();
    constexpr std::tuple<uint32_t, bool, float> the_return{10, true, 1.2};
    ASSERT_EQ(the_return, ret1.value());
}

TEST_F(LuaIntfTest, TestLuaCheckType) {
    intf.pushStack(true);
    ASSERT_TRUE(intf.luaCheckType<bool>(-1));
    ASSERT_FALSE(intf.luaCheckType<int>(-1));
    intf.luaPop(1);

    intf.pushStack(12);
    ASSERT_TRUE(intf.luaCheckType<int>(-1));
    ASSERT_FALSE(intf.luaCheckType<bool>(-1));
    intf.luaPop(1);

    intf.pushStack("string literal");
    ASSERT_TRUE(intf.luaCheckType<std::string>(-1));
    ASSERT_FALSE(intf.luaCheckType<bool>(-1));
    intf.luaPop(1);

    intf.pushStack(std::string("std::string"));
    ASSERT_TRUE(intf.luaCheckType<std::string>(-1));
    ASSERT_FALSE(intf.luaCheckType<bool>(-1));
    intf.luaPop(1);

    intf.pushStack(nullptr);
    ASSERT_TRUE(intf.luaCheckType<std::nullptr_t>(-1));
    ASSERT_FALSE(intf.luaCheckType<bool>(-1));
    intf.luaPop(1);

    intf.pushStack(1.2f);
    ASSERT_TRUE(intf.luaCheckType<float>(-1));
    // ASSERT_FALSE(intf.luaCheckType<std::string>(-1)); // TODO: should be false
    intf.luaPop(1);
}
