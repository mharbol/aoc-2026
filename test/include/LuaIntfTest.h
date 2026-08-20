
#ifndef __LUA_TEST_H__
#define __LUA_TEST_H__

#include "LuaIntf.h"
#include <gtest/gtest.h>

class LuaIntfTest : public ::testing::Test {
protected:
    aoc::lua::LuaIntf intf{};
};

#endif
