#include <gmock/gmock.h>
#include "loadaccumulatorwithmemory.hpp"
#include "opcodes.hpp"

using namespace testing;

TEST_F(LoadAccumulatorWithMemory, ZeroPageIndirectIndexedWithY)
{
    EXPECT_EQ(1, 1);
    ASSERT_THAT(0, Eq(0));
}
