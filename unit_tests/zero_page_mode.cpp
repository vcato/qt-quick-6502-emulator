#include <gmock/gmock.h>

using namespace testing;

TEST(LoadAccumulatorWithMemory, ZeroPage)
{
    EXPECT_EQ(1, 1);
    ASSERT_THAT(0, Eq(0));
}
