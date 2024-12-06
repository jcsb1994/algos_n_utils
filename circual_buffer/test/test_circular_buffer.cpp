/*! \author jcsb1994
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    \brief Test for circular_buffer

    Test single byte write and 3 types of write.
*/

#include <stdint.h>
#include <iostream>
#include "circular_buffer.h"
#include "debug_tools.h"
#include "anu_test.h"

int main()
{
    anu_test_print_title();

    circular_buffer<10> myBuff;
    uint8_t write[12];
    uint8_t readback[10];
    for (size_t i = 0; i < 12; i++) {
        write[i] = i;
    }


    std::cout << "Test R/W singe bytes\n";
    std::cout << "write_byte(1) RC: " << myBuff.write_byte(1) << "\n";
    std::cout << "read_byte RC: " << myBuff.read_byte(readback) << "\n";
    std::cout << "read_byte ?= 1: " << (int)readback[0] << "\n";

    std::cout << "\nTest R/W whole array\n";
    std::cout << "write(4, whole) RC: " << myBuff.write(write, 4,
        circular_buffer<10>::write_operation::whole) << "\n";
    std::cout << "write(12, whole) RC: " << myBuff.write(write, 12,
        circular_buffer<10>::write_operation::whole) << "\n";
    std::cout << "get_available ?= 6: " << myBuff.get_available_len() << "\n";
    std::cout << "read() RC: " << myBuff.read(readback, 4) << "\n";
    for (size_t i = 0; i < 4; i++) {
        std::cout << (int)readback[i] << ", ";
    }
    std::cout<< "\n";

    std::cout << "\nTest R/W array overflow\n";
    std::cout << "write(12, fill) RC ?= 10: " << myBuff.write(write, 12,
        circular_buffer<10>::write_operation::fill) << "\n";
    std::cout << "write(12, overflow) RC ?= 12: " << myBuff.write(write, 12,
        circular_buffer<10>::write_operation::overflow) << "\n";

    return 0;
}
