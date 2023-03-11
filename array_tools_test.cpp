/*! \author jcsb1994
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    \brief Test for array_tools
*/


#include <stdio.h>
#include <stdint.h>
#include "array_tools.h"

uint8_t answers[6];

int main()
{
    uint8_t myBuffer[6] = {0,1,2,3,4,5};
    // Test that pos 1 outputs idx1 with value 100
    for (uint8_t i = 0; i < 6; i++) {
        answers[i] = array_tools::get_nth_largest_elem_idx<uint8_t>(myBuffer, 6, i);
    }
    return 0;
}