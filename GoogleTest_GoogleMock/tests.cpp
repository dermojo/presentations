#include "stringify.hpp"

#include <gtest/gtest.h>

TEST(Stringify, to_string1)
{
    Stringify str;
    ASSERT_TRUE(str.to_string(5) == "x5");
}

TEST(Stringify, to_string2)
{
    Stringify str;
    ASSERT_EQ(str.to_string(123), "123");
}

TEST(Stringify, to_string3)
{
    Stringify str;
    EXPECT_EQ(str.to_string(4711), "4711");
    EXPECT_EQ(str.to_string(0), "0");
    EXPECT_EQ(str.to_string(-12), "-12");
    // ...
}

class StringifyTest : public testing::Test
{
protected:
    // StringifyTest() = default;
    // ~StringifyTest() override = default;
    // void SetUp() override {}
    // void TearDown() override {}
    Stringify str;
};

TEST_F(StringifyTest, to_string4)
{
    EXPECT_EQ(str.to_string(4711), "4711");
    EXPECT_EQ(str.to_string(0), "0");
    EXPECT_EQ(str.to_string(-12), "-12");
    // ...
}
