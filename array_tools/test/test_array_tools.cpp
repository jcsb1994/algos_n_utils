/*! \author jcsb1994
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    \brief Test for array_tools
*/


#include <stdint.h>
#include <iostream>
#include "array_tools.h"
#include "debug_tools.h"
#include "anu_test.h"

#define TEST_SIZE 6
int answers[TEST_SIZE];

int main()
{
    anu_test_print_title();

    uint8_t myBuffer[TEST_SIZE] = {0,1,2,3,4,5};
    std::cout << "\nTest 1: ";
    for (uint8_t i = 0; i < TEST_SIZE; i++) {
        answers[i] = array_tools::get_nth_largest_elem_idx<uint8_t>(myBuffer, TEST_SIZE, i);
        std::cout << answers[i] << ", ";
    }

    uint8_t myBuffer2[TEST_SIZE] = {1,1,2,2,3,3};
    std::cout << "\nTest 2: ";
    for (uint8_t i = 0; i < TEST_SIZE; i++) {
        answers[i] = array_tools::get_nth_largest_elem_idx<uint8_t>(myBuffer2, TEST_SIZE, i);
        std::cout << answers[i] << ", ";
    }
    std::cout << "\n";
    return 0;
}
