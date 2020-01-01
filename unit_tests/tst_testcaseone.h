#ifndef TST_TESTCASEONE_H
#define TST_TESTCASEONE_H

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

using namespace testing;

TEST(UnitTests, TestCaseOne)
{
    EXPECT_EQ(1, 1);
    ASSERT_THAT(0, Eq(0));
}

#endif // TST_TESTCASEONE_H
