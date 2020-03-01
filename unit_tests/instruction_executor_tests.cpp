#include <gmock/gmock.h>
#include "InstructionExecutorTestFixture.hpp"
#include <limits>

using namespace testing;

TEST_F(InstructionExecutorTestFixture, CompleteReturnsTrueUponInitialization)
{
    EXPECT_THAT(executor.complete(), Eq(true));
}

TEST_F(InstructionExecutorTestFixture, ClockTicksReturnsZeroUponInitialization)
{
    EXPECT_THAT(executor.clock_ticks, Eq(0U));
}

/** Verify that calling clock() increments clock_ticks by one.
 *
 *  Basic strategy: Verify the behavior at the two extremes
 *    ) 0 to 1  (min to one greater)
 *    ) max to 0  (max back around to min)
 *
 *  @pre The clock_ticks member is an unsigned variable.
 */
TEST_F(InstructionExecutorTestFixture, ClockTickIncrementsClockCount)
{
    // Test the type expectations, so that we expect a rollover...
    EXPECT_THAT(std::numeric_limits<typeof executor.clock_ticks>::is_integer, Eq(true));
    EXPECT_THAT(std::numeric_limits<typeof executor.clock_ticks>::is_signed, Eq(false));

    // Lowest value case
    EXPECT_THAT(executor.clock_ticks, Eq(std::numeric_limits<typeof executor.clock_ticks>::min()));

    executor.clock();

    EXPECT_THAT(executor.clock_ticks, Eq(std::numeric_limits<typeof executor.clock_ticks>::min() + 1));

    // Try the rollover case.
    executor.clock_ticks = std::numeric_limits<typeof executor.clock_ticks>::max();

    EXPECT_THAT(executor.clock_ticks, Eq(std::numeric_limits<typeof executor.clock_ticks>::max()));

    executor.clock();

    EXPECT_THAT(executor.clock_ticks, Eq(std::numeric_limits<typeof executor.clock_ticks>::min()));
}
