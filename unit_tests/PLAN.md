# Unit Test Plan

This file is mainly for keeping track of ideas for an overall Unit Testing plan.

## Basic Ideas

* Testing an instruction at an address
* Testing an instruction at multiple points **within** a single page of memory:
    * Bottom
    * Top
    * Somewhere in-between
* Testing an instruction **crossing** a page boundary
    * 2-byte instruction
        * Only 1 way to do this.  The parameter is on the next page.
    * 3-byte instruction (2 ways
        * 2 ways to do this.  Once for each byte of the parameter being on the next page.
* Testing status flags: boolean, so two states to test
    * N
    * Z
    * C
        * Arithmetic operations
        * Shifting
    * V
        * Only a few instructions use this.  Happens when instruction causes wrap-around of a number.

## Simplifying Tests

Consider the role of having instruction "properties" that can have their own check, independent
of the instruction.  Could make the tests easier to read and comprehend.

Examples:
* Program counter is incremented by the instruction size in memory
* Memory contains expected instruction and parameter
