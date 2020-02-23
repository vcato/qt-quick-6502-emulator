#include <gmock/gmock.h>
#include "registers.hpp"


using namespace testing;

/** Demonstrates that the default constructed Registers has zeros in all registers.
 *
 */
TEST(Registers, DefaultConstructHasDataAsAllZeros)
{
    Registers registers;

    EXPECT_THAT(registers.a, Eq(0)) << "accumulator is not zero";
    EXPECT_THAT(registers.x, Eq(0)) << "x is not zero";
    EXPECT_THAT(registers.y, Eq(0)) << "y is not zero";
    EXPECT_THAT(registers.stack_pointer, Eq(0)) << "stack pointer is not zero";
    EXPECT_THAT(registers.program_counter, Eq(0)) << "program counter is not zero";
    EXPECT_THAT(registers.status, Eq(0)) << "status is not zero";
}

/** Demonstrate that calling SetFlag with the individual flags sets that flag in the status register.
 *
 *  First, we will test each individual bit, followed by multiple bits at once, to
 *  demonstrate that only a particular bit is affected.
 *
 *  It is important here to test for the change returned by GetFlag, so we must have the flag in one
 *  state, test that GetFlag() returns that state, then set the flag and test again to detect the flip.
 *  Finally, we set again to detect the reverse flip (going back to the original state).
 */
TEST(Registers, SetFlagSetsStatus)
{
    Registers registers;

    EXPECT_THAT(registers.status, Eq(0));

    // Individual bits!

    // Carry flag
    {
        // Setting the Carry bit causes the bit to be set in status.
        registers.SetFlag(C, 1);
        EXPECT_THAT(registers.status, Eq(C));

        // Clear the flag
        registers.SetFlag(C, 0);
        EXPECT_THAT(registers.status, Eq(0));
    }

    // Zero flag
    {
        // Setting the Zero bit causes the bit to be set in status.
        registers.SetFlag(Z, 1);
        EXPECT_THAT(registers.status, Eq(Z));

        // Clear the flag
        registers.SetFlag(Z, 0);
        EXPECT_THAT(registers.status, Eq(0));
    }

    // Interrupt flag
    {
        // Setting the Interrupt bit causes the bit to be set in status.
        registers.SetFlag(I, 1);
        EXPECT_THAT(registers.status, Eq(I));

        // Clear the flag
        registers.SetFlag(I, 0);
        EXPECT_THAT(registers.status, Eq(0));
    }

    // Decimal flag
    {
        // Setting the Decimal bit causes the bit to be set in status.
        registers.SetFlag(D, 1);
        EXPECT_THAT(registers.status, Eq(D));

        // Clear the flag
        registers.SetFlag(D, 0);
        EXPECT_THAT(registers.status, Eq(0));
    }

    // Break flag
    {
        // Setting the Break bit causes the bit to be set in status.
        registers.SetFlag(B, 1);
        EXPECT_THAT(registers.status, Eq(B));

        // Clear the flag
        registers.SetFlag(B, 0);
        EXPECT_THAT(registers.status, Eq(0));
    }

    // Unused flag
    {
        // Setting the Break bit causes the bit to be set in status.
        registers.SetFlag(U, 1);
        EXPECT_THAT(registers.status, Eq(U));

        // Clear the flag
        registers.SetFlag(U, 0);
        EXPECT_THAT(registers.status, Eq(0));
    }

    // Overflow flag
    {
        // Setting the Overflow bit causes the bit to be set in status.
        registers.SetFlag(V, 1);
        EXPECT_THAT(registers.status, Eq(V));

        // Clear the flag
        registers.SetFlag(V, 0);
        EXPECT_THAT(registers.status, Eq(0));
    }

    // Negative flag
    {
        // Setting the Negative bit causes the bit to be set in status.
        registers.SetFlag(N, 1);
        EXPECT_THAT(registers.status, Eq(N));

        // Clear the flag
        registers.SetFlag(N, 0);
        EXPECT_THAT(registers.status, Eq(0));
    }

    EXPECT_THAT(registers.status, Eq(0)) << "status register is not zero for second half of test";

    // Now we test combinations!
    {
        registers.SetFlag(C, 1);
        registers.SetFlag(Z, 1);
        EXPECT_THAT(registers.status, Eq(C | Z)) << "expected the C and Z flags to be set";

        registers.SetFlag(I, 1);
        EXPECT_THAT(registers.status, Eq(C | Z | I)) << "expected the C, Z, and I flags to be set";

        registers.SetFlag(D, 1);
        EXPECT_THAT(registers.status, Eq(C | Z | I | D)) << "expected the C, Z, I, and D flags to be set";

        registers.SetFlag(B, 1);
        EXPECT_THAT(registers.status, Eq(C | Z | I | D | B)) << "expected the C, Z, I, D, and B flags to be set";

        registers.SetFlag(U, 1);
        EXPECT_THAT(registers.status, Eq(C | Z | I | D | B | U)) << "expected the C, Z, I, D, B, and U flags to be set";

        registers.SetFlag(V, 1);
        EXPECT_THAT(registers.status, Eq(C | Z | I | D | B | U | V)) << "expected the C, Z, I, D, B, U, and V flags to be set";

        registers.SetFlag(N, 1);
        EXPECT_THAT(registers.status, Eq(C | Z | I | D | B | U | V | N)) << "expected the C, Z, I, D, B, U, V, and N flags to be set";
    }
}

/** Demonstrate that calling GetFlag with the individual flags gets that flag in the status register.
 *
 *  First, we will test each individual bit, followed by multiple bits at once, to
 *  demonstrate that only a particular bit is fetched.
 *
 *  It is important here to test for the change returned by GetFlag, so we must have the flag in one
 *  state, test that GetFlag() returns that state, then set the flag and test again to detect the flip.
 *  Finally, we set again to detect the reverse flip (going back to the original state).
 *
 *  In the second half, we will check that GetFlag will work correctly when multiple flags
 *  are set by first setting all the flags and doing a check for each flag being set.
 *  Then we clear flags one by one and checking each flag for its current state.
 */
TEST(Registers, GetFlagGetsStatus)
{
    Registers registers;

    EXPECT_THAT(registers.status, Eq(0));

    // Individual bits!

    // Carry flag
    {
        EXPECT_THAT(registers.GetFlag(C), Eq(false));

        // Getting the Carry bit causes the bit to be fetched in status.
        registers.status = C;

        EXPECT_THAT(registers.GetFlag(C), Eq(true));

        // Clear the flag
        registers.status = 0;
        EXPECT_THAT(registers.GetFlag(C), Eq(false));
    }

    // Zero flag
    {
        EXPECT_THAT(registers.GetFlag(Z), Eq(false));

        // Getting the Zero bit causes the bit to be fetched in status.
        registers.status = Z;
        EXPECT_THAT(registers.GetFlag(Z), Eq(true));

        // Clear the flag
        registers.status = 0;
        EXPECT_THAT(registers.GetFlag(Z), Eq(false));
    }

    // Interrupt flag
    {
        EXPECT_THAT(registers.GetFlag(I), Eq(false));

        // Getting the Interrupt bit causes the bit to be fetched in status.
        registers.status = I;
        EXPECT_THAT(registers.GetFlag(I), Eq(true));

        // Clear the flag
        registers.status = 0;
        EXPECT_THAT(registers.GetFlag(I), Eq(false));
    }

    // Decimal flag
    {
        EXPECT_THAT(registers.GetFlag(D), Eq(false));

        // Getting the Decimal bit causes the bit to be fetched in status.
        registers.status = D;
        EXPECT_THAT(registers.GetFlag(D), Eq(true));

        // Clear the flag
        registers.status = 0;
        EXPECT_THAT(registers.GetFlag(D), Eq(false));
    }

    // Break flag
    {
        EXPECT_THAT(registers.GetFlag(B), Eq(false));

        // Getting the Break bit causes the bit to be fetched in status.
        registers.status = B;
        EXPECT_THAT(registers.GetFlag(B), Eq(true));

        // Clear the flag
        registers.status = 0;
        EXPECT_THAT(registers.GetFlag(B), Eq(false));
    }

    // Unused flag
    {
        EXPECT_THAT(registers.GetFlag(U), Eq(false));

        // Getting the Unused bit causes the bit to be fetched in status.
        registers.status = U;
        EXPECT_THAT(registers.GetFlag(U), Eq(true));

        // Clear the flag
        registers.status = 0;
        EXPECT_THAT(registers.GetFlag(U), Eq(false));
    }

    // Overflow flag
    {
        EXPECT_THAT(registers.GetFlag(V), Eq(false));

        // Getting the Overflow bit causes the bit to be fetched in status.
        registers.status = V;
        EXPECT_THAT(registers.GetFlag(V), Eq(true));

        // Clear the flag
        registers.status = 0;
        EXPECT_THAT(registers.GetFlag(V), Eq(false));
    }

    // Negative flag
    {
        EXPECT_THAT(registers.GetFlag(N), Eq(false));

        // Getting the Negative bit causes the bit to be fetched in status.
        registers.status = N;
        EXPECT_THAT(registers.GetFlag(N), Eq(true));

        // Clear the flag
        registers.status = 0;
        EXPECT_THAT(registers.GetFlag(N), Eq(false));
    }

    EXPECT_THAT(registers.status, Eq(0)) << "status register is not zero for second half of test";

    // Now we test combinations!
    {
        registers.status = C | Z | I | D | B | U | V | N;

        EXPECT_THAT(registers.GetFlag(C), Eq(true));
        EXPECT_THAT(registers.GetFlag(Z), Eq(true));
        EXPECT_THAT(registers.GetFlag(I), Eq(true));
        EXPECT_THAT(registers.GetFlag(D), Eq(true));
        EXPECT_THAT(registers.GetFlag(B), Eq(true));
        EXPECT_THAT(registers.GetFlag(U), Eq(true));
        EXPECT_THAT(registers.GetFlag(V), Eq(true));
        EXPECT_THAT(registers.GetFlag(N), Eq(true));

        registers.status = C | Z | I | D | B | U | V;

        EXPECT_THAT(registers.GetFlag(C), Eq(true));
        EXPECT_THAT(registers.GetFlag(Z), Eq(true));
        EXPECT_THAT(registers.GetFlag(I), Eq(true));
        EXPECT_THAT(registers.GetFlag(D), Eq(true));
        EXPECT_THAT(registers.GetFlag(B), Eq(true));
        EXPECT_THAT(registers.GetFlag(U), Eq(true));
        EXPECT_THAT(registers.GetFlag(V), Eq(true));
        EXPECT_THAT(registers.GetFlag(N), Eq(false));

        registers.status = C | Z | I | D | B | U;

        EXPECT_THAT(registers.GetFlag(C), Eq(true));
        EXPECT_THAT(registers.GetFlag(Z), Eq(true));
        EXPECT_THAT(registers.GetFlag(I), Eq(true));
        EXPECT_THAT(registers.GetFlag(D), Eq(true));
        EXPECT_THAT(registers.GetFlag(B), Eq(true));
        EXPECT_THAT(registers.GetFlag(U), Eq(true));
        EXPECT_THAT(registers.GetFlag(V), Eq(false));
        EXPECT_THAT(registers.GetFlag(N), Eq(false));

        registers.status = C | Z | I | D | B;

        EXPECT_THAT(registers.GetFlag(C), Eq(true));
        EXPECT_THAT(registers.GetFlag(Z), Eq(true));
        EXPECT_THAT(registers.GetFlag(I), Eq(true));
        EXPECT_THAT(registers.GetFlag(D), Eq(true));
        EXPECT_THAT(registers.GetFlag(B), Eq(true));
        EXPECT_THAT(registers.GetFlag(U), Eq(false));
        EXPECT_THAT(registers.GetFlag(V), Eq(false));
        EXPECT_THAT(registers.GetFlag(N), Eq(false));

        registers.status = C | Z | I | D;

        EXPECT_THAT(registers.GetFlag(C), Eq(true));
        EXPECT_THAT(registers.GetFlag(Z), Eq(true));
        EXPECT_THAT(registers.GetFlag(I), Eq(true));
        EXPECT_THAT(registers.GetFlag(D), Eq(true));
        EXPECT_THAT(registers.GetFlag(B), Eq(false));
        EXPECT_THAT(registers.GetFlag(U), Eq(false));
        EXPECT_THAT(registers.GetFlag(V), Eq(false));
        EXPECT_THAT(registers.GetFlag(N), Eq(false));

        registers.status = C | Z | I;

        EXPECT_THAT(registers.GetFlag(C), Eq(true));
        EXPECT_THAT(registers.GetFlag(Z), Eq(true));
        EXPECT_THAT(registers.GetFlag(I), Eq(true));
        EXPECT_THAT(registers.GetFlag(D), Eq(false));
        EXPECT_THAT(registers.GetFlag(B), Eq(false));
        EXPECT_THAT(registers.GetFlag(U), Eq(false));
        EXPECT_THAT(registers.GetFlag(V), Eq(false));
        EXPECT_THAT(registers.GetFlag(N), Eq(false));

        registers.status = C | Z;

        EXPECT_THAT(registers.GetFlag(C), Eq(true));
        EXPECT_THAT(registers.GetFlag(Z), Eq(true));
        EXPECT_THAT(registers.GetFlag(I), Eq(false));
        EXPECT_THAT(registers.GetFlag(D), Eq(false));
        EXPECT_THAT(registers.GetFlag(B), Eq(false));
        EXPECT_THAT(registers.GetFlag(U), Eq(false));
        EXPECT_THAT(registers.GetFlag(V), Eq(false));
        EXPECT_THAT(registers.GetFlag(N), Eq(false));

        registers.status = C;

        EXPECT_THAT(registers.GetFlag(C), Eq(true));
        EXPECT_THAT(registers.GetFlag(Z), Eq(false));
        EXPECT_THAT(registers.GetFlag(I), Eq(false));
        EXPECT_THAT(registers.GetFlag(D), Eq(false));
        EXPECT_THAT(registers.GetFlag(B), Eq(false));
        EXPECT_THAT(registers.GetFlag(U), Eq(false));
        EXPECT_THAT(registers.GetFlag(V), Eq(false));
        EXPECT_THAT(registers.GetFlag(N), Eq(false));

        registers.status = 0;

        EXPECT_THAT(registers.GetFlag(C), Eq(false));
        EXPECT_THAT(registers.GetFlag(Z), Eq(false));
        EXPECT_THAT(registers.GetFlag(I), Eq(false));
        EXPECT_THAT(registers.GetFlag(D), Eq(false));
        EXPECT_THAT(registers.GetFlag(B), Eq(false));
        EXPECT_THAT(registers.GetFlag(U), Eq(false));
        EXPECT_THAT(registers.GetFlag(V), Eq(false));
        EXPECT_THAT(registers.GetFlag(N), Eq(false));
    }
}

/** Demonstrate that given the same flag, GetFlag Will retrieve the given flag set by SetFlag.
 *
 */
TEST(Registers, GetFlagAndSetFlagAreRelated)
{
    Registers registers;

    EXPECT_THAT(registers.GetFlag(C), Eq(false));
    registers.SetFlag(C, 1);
    EXPECT_THAT(registers.GetFlag(C), Eq(true));
    registers.SetFlag(C, 0);
    EXPECT_THAT(registers.GetFlag(C), Eq(false));

    EXPECT_THAT(registers.GetFlag(Z), Eq(false));
    registers.SetFlag(Z, 1);
    EXPECT_THAT(registers.GetFlag(Z), Eq(true));
    registers.SetFlag(Z, 0);
    EXPECT_THAT(registers.GetFlag(Z), Eq(false));

    EXPECT_THAT(registers.GetFlag(I), Eq(false));
    registers.SetFlag(I, 1);
    EXPECT_THAT(registers.GetFlag(I), Eq(true));
    registers.SetFlag(I, 0);
    EXPECT_THAT(registers.GetFlag(I), Eq(false));

    EXPECT_THAT(registers.GetFlag(D), Eq(false));
    registers.SetFlag(D, 1);
    EXPECT_THAT(registers.GetFlag(D), Eq(true));
    registers.SetFlag(D, 0);
    EXPECT_THAT(registers.GetFlag(D), Eq(false));

    EXPECT_THAT(registers.GetFlag(B), Eq(false));
    registers.SetFlag(B, 1);
    EXPECT_THAT(registers.GetFlag(B), Eq(true));
    registers.SetFlag(B, 0);
    EXPECT_THAT(registers.GetFlag(B), Eq(false));

    EXPECT_THAT(registers.GetFlag(U), Eq(false));
    registers.SetFlag(U, 1);
    EXPECT_THAT(registers.GetFlag(U), Eq(true));
    registers.SetFlag(U, 0);
    EXPECT_THAT(registers.GetFlag(U), Eq(false));

    EXPECT_THAT(registers.GetFlag(V), Eq(false));
    registers.SetFlag(V, 1);
    EXPECT_THAT(registers.GetFlag(V), Eq(true));
    registers.SetFlag(V, 0);
    EXPECT_THAT(registers.GetFlag(V), Eq(false));

    EXPECT_THAT(registers.GetFlag(N), Eq(false));
    registers.SetFlag(N, 1);
    EXPECT_THAT(registers.GetFlag(N), Eq(true));
    registers.SetFlag(N, 0);
    EXPECT_THAT(registers.GetFlag(N), Eq(false));
}
