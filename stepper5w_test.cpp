#include <iostream>
#include "stepper5w.h"

stepper5w myStepper(2048);

void step_callback(uint8_t coil_state);

int main()
{
    myStepper.set_step_cb(step_callback);
    std::cout << "\nTest 1; Stepper step once: ";
    myStepper.step();
    // Va surement falloir faire fct qui fait 8 steps bk/forth pr que moteur soit au bon step

    return 0;
}


void step_callback(uint8_t coil_state)
{
    std::cout << "Coils are "
        << int(coil_state & 1)
        << int((coil_state>>1) & 1)
        << int((coil_state>>2) & 1)
        << int((coil_state>>3) & 1)
        << "\n";
}