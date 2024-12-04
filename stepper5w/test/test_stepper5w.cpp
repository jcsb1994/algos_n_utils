#include <iostream>
#include "stepper5w.h"

stepper5w myStepper(48);

void step_callback(uint8_t coil_state);

int main()
{
    myStepper.set_step_cb(step_callback);
    std::cout << "\nTest 1; Stepper step once:\n";
    myStepper.step(1);
    // Va surement falloir faire fct qui fait 8 steps bk/forth pr que moteur soit au bon step

    std::cout << "\nTest 2; Stepper 8 steps:\n";
    for (uint8_t i = 0; i < 8; i++)
    {
        myStepper.step(1);
    }

    std::cout << "\nTest 3; Stepper 120 degrees:\n";
    myStepper.rotate(1, 120);
    while (!myStepper.run()) {}

    return 0;
}


void step_callback(uint8_t coil_state)
{
    std::cout << "Coils are "
        << int((coil_state>>3) & 1)
        << int((coil_state>>2) & 1)
        << int((coil_state>>1) & 1)
        << int(coil_state & 1)
        << "\n";
}
