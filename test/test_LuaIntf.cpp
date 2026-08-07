
#include <gtest/gtest.h>

#include "LuaIntf.h"

TEST(TestHello, Hello) {
    ASSERT_EQ(2, 1 + 1) << "This should clearly be 2";
}

TEST(TestHello, CreateIntf) {
    aoc::lua::LuaIntf _{};
    SUCCEED();
}
