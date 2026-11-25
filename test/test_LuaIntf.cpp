
#include <array>
#include <cstdint>
#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <tuple>
#include <vector>

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

TEST_F(LuaIntfStackTest, TestPushStackVec) {

    const std::vector<int32_t> vec_primes{2, 3, 5, 7, 11};
    intf.pushStack(vec_primes);

    const int32_t arr_squares[]{1, 4, 9, 16, 25};
    intf.pushStack(arr_squares);

    const std::array<std::string, 4> arr_str{"a", "b", "c", "d"};
    intf.pushStack(arr_str);

    intf.luaPop(3);
}

TEST_F(LuaIntfStackTest, TestPushPopStackVec) {

    const std::vector<uint32_t> vec_primes{2, 3, 5, 7, 11};
    const std::vector<int32_t> vec_squares{1, 4, 9, 16, 25};
    const std::array<std::string, 4> arr_str{"a", "b", "c", "d"};

    intf.pushStack(vec_primes);
    intf.pushStack(vec_squares);
    intf.pushStack(arr_str);

    const auto ret_strs_vec = intf.popStack<std::vector<std::string>>();
    ASSERT_EQ(std::vector<std::string>(arr_str.begin(), arr_str.end()), *ret_strs_vec);
    const auto ret_vec_squares = intf.popStack<std::vector<int32_t>>();
    ASSERT_EQ(vec_squares, *ret_vec_squares);
    const auto ret_vec_primes = intf.popStack<std::vector<uint32_t>>();
    ASSERT_EQ(vec_primes, *ret_vec_primes);
}

TEST_F(LuaIntfStackTest, TestCallVecFunc) {
    LUA_DO_STRING("function sum(arr)"
                  "    local sum = 0"
                  "    for _, val in ipairs(arr) do"
                  "        sum = sum + val"
                  "    end"
                  "    return sum "
                  "end");
    intf.openBaseLibs();
    const std::vector<uint32_t> nums{1, 2, 3, 4, 5};
    const auto ret = intf.call<uint32_t>("sum", nums);
    ASSERT_TRUE(ret.has_value()) << "Expect ret should have a value: " << ret.error();
    ASSERT_EQ(15, *ret);
}

TEST_F(LuaIntfStackTest, TestBasicDoString) {
    LUA_DO_STRING("a = 2 + 2");
    const auto a = intf.getGlobal<uint32_t>("a");
    ASSERT_TRUE(a.has_value()) << "Expect 'a' should be a global value: " << a.error();
    ASSERT_EQ(4, a.value());
}

TEST_F(LuaIntfStackTest, TestCallSimpleFunc) {
    LUA_DO_STRING("function retVal(a) return a end");
    auto ret = intf.call<uint32_t>("retVal", 10);
    ASSERT_TRUE(ret.has_value()) << "Expect ret should be a value: " << ret.error();
    ASSERT_EQ(10, ret.value());
}

TEST_F(LuaIntfStackTest, TestCallAddFunc) {
    LUA_DO_STRING("function retVal(a, b) return a + b end");
    auto ret = intf.call<uint32_t>("retVal", 5, 5);
    ASSERT_TRUE(ret.has_value()) << "Expect ret should be a value: " << ret.error();
    ASSERT_EQ(10, ret.value());
}

TEST_F(LuaIntfStackTest, TestCallBiggerFunction) {
    LUA_DO_STRING("function add(a, b) return a + b end");
    LUA_DO_STRING("function retVal(a, b) return add(a, b) end");
    auto ret = intf.call<uint32_t>("retVal", 5, 5);
    ASSERT_TRUE(ret.has_value()) << "Expect ret should be a value: " << ret.error();
    ASSERT_EQ(10, ret.value());
}

TEST_F(LuaIntfStackTest, TestCallDivFunction) {
    LUA_DO_STRING("function retVal(a, b) return a // b end");
    auto ret = intf.call<uint32_t>("retVal", 12, 6);
    ASSERT_TRUE(ret.has_value()) << "Expect ret should be a value: " << ret.error();
    ASSERT_EQ(2, ret.value());
}

TEST_F(LuaIntfStackTest, TestCallNoArgsFunction) {
    LUA_DO_STRING("function retVal() return 10 end");
    auto ret = intf.call<uint32_t>("retVal");
    ASSERT_TRUE(ret.has_value()) << "Expect ret should be a value: " << ret.error();
    ASSERT_EQ(10, ret.value());
}

TEST_F(LuaIntfStackTest, TestCallStringFunction) {
    LUA_DO_STRING("function retVal(a) return #a end");
    auto ret = intf.call<uint32_t>("retVal", "0123456789");
    ASSERT_TRUE(ret.has_value()) << "Expect ret should be a value: " << ret.error();
    ASSERT_EQ(10, ret.value());
}

TEST_F(LuaIntfStackTest, TestCallMixedFunction) {
    LUA_DO_STRING("function retVal(str, num) return #str + num end");
    auto ret = intf.call<uint32_t>("retVal", "0123456789", 10);
    ASSERT_TRUE(ret.has_value()) << "Expect ret should be a value: " << ret.error();
    ASSERT_EQ(20, ret.value());
}

TEST_F(LuaIntfStackTest, TestCallVoidFunction) {
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

TEST_F(LuaIntfStackTest, TestCallMultiRetFunction) {
    LUA_DO_STRING("function retTup() return 1, 2, 3 end");
    const auto ret1 = intf.call<uint32_t, uint32_t, uint32_t>("retTup").value();
    ASSERT_EQ(std::make_tuple(1, 2, 3), ret1);

    LUA_DO_STRING("function retTupMixed() return false, 2, 'hello' end");
    const auto ret2 = intf.call<bool, int32_t, std::string>("retTupMixed").value();
    ASSERT_EQ(std::make_tuple(false, 2, "hello"), ret2);
}

TEST_F(LuaIntfStackTest, TestCallStringRetFunction) {
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

    const auto ret1 = intf.luaGetStack<int>(-1);
    ASSERT_TRUE(ret1.has_value()) << "Expect ret1 should be a value: " << ret1.error();
    ASSERT_EQ(10, ret1.value());

    const auto ret1_optional = intf.luaGetStack<std::optional<int>>(-1);
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

TEST_F(LuaIntfStackTest, TestLuaPopStack) {
    intf.pushStack(10); // -1
    auto ret1 = intf.popStack<uint32_t>();
    ASSERT_TRUE(ret1.has_value()) << "Expect ret1 should be a value: " << ret1.error();
    ASSERT_EQ(10, ret1.value());
}

TEST_F(LuaIntfStackTest, TestLuaPopStackVoid) {

    const size_t init_stack_size = intf.getStackSize();

    intf.pushStack(10); // -1
    auto ret1 = intf.popStack<void>();
    ASSERT_TRUE(ret1.has_value()) << "Expect ret1 should be a value: " << ret1.error();

    ASSERT_EQ(init_stack_size + 1, intf.getStackSize())
        << "Stack should be one larger than initial";

    auto ret2 = intf.popStack<uint32_t>();
    ASSERT_TRUE(ret2.has_value()) << "Expect ret2 should be a value: " << ret1.error();
    ASSERT_EQ(10, ret2.value());
}

TEST_F(LuaIntfStackTest, TestLuaPopMultiStack) {
    intf.pushStack(1.2f); // -3
    intf.pushStack(true); // -2
    intf.pushStack(10);   // -1
    auto ret = intf.popStack<float, bool, uint32_t>();
    ASSERT_TRUE(ret.has_value()) << "Expect ret should be a value: " << ret.error();
    constexpr std::tuple<float, bool, uint32_t> the_return{1.2, true, 10};
    ASSERT_EQ(the_return, ret.value());
}

TEST_F(LuaIntfStackTest, TestLuaCheckType) {
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

// TODO, this is eventually private
TEST_F(LuaIntfTest, TestLuaGetUncheckedStack) {
    intf.pushStack(12);
    const auto ret = intf.luaGetStackUnchecked<int>(-1);
    ASSERT_EQ(12, ret);
    const auto ret2 = intf.luaGetStackUnchecked<float>(-1);
    ASSERT_EQ(12.0, ret2);
    const auto ret3 = intf.luaGetStackUnchecked<bool>(-1);
    ASSERT_EQ(true, ret3); // even though this is an int... "unchecked" yah know...
}

TEST_F(LuaIntfTest, TestLuaVariantStack) {
    intf.pushStack(12);
    const auto ret = intf.luaGetStackVariant<int>(-1);
    ASSERT_TRUE(ret.has_value()) << "Expect ret should be a value: " << ret.error();
    ASSERT_EQ(12, std::get<0>(*ret));

    const auto ret2 = intf.luaGetStackVariant<bool>(-1);
    ASSERT_FALSE(ret2.has_value()) << "Expect ret2 should not be a value: " << ret2.error();

    const auto ret3 = intf.luaGetStackVariant<bool, std::string>(-1);
    ASSERT_TRUE(ret3.has_value()) << "Expect ret3 should be a value: " << ret3.error();
    ASSERT_EQ("12", std::get<1>(*ret3));
}

TEST_F(LuaIntfStackTest, TestLuaVec) {

    const std::vector<uint32_t> nums{1, 2, 3};
    const std::vector<std::string> strs{"raindrops", "roses", "whiskers", "kittens"};

    intf.pushStack(nums);
    ASSERT_TRUE(intf.luaCheckType<std::vector<int>>(-1));
    intf.pushStack(strs);
    // TODO, this should be false since it is std::strings...
    ASSERT_TRUE(intf.luaCheckType<std::vector<int>>(-1));

    const auto ret_nums = intf.luaGetStack<std::vector<uint32_t>>(-2);
    ASSERT_EQ(nums, *ret_nums);

    const auto ret_strs = intf.luaGetStack<std::vector<std::string>>(-1);
    ASSERT_EQ(strs, *ret_strs);

    intf.luaPop(2);
}
