
#include <gtest/gtest.h>

#include "LuaIntf.h"
#include "LuaIntfTest.h"

TEST(TestLua, CreateIntf) {
    aoc::lua::LuaIntf _{};
    SUCCEED();
}

TEST_F(LuaIntfTest, TestBasicDoString) {
    intf.luaDoString("a = 2 + 2");
    const auto a = intf.getGlobal<int32_t>("a");
    ASSERT_TRUE(a.has_value()) << "Expect 'a' should be a global value";
    ASSERT_EQ(4, a.value()) << "Expect 'a' should be 4";
}
