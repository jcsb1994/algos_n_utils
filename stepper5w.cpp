#include "stepper5w.h"


void stepper5w::step()
{
    if (_step_cb == 0) {
        return;
    }
    _step_cb(_step_table[_step++]);
    _step &= (STEPPER5W_STEPS_NB-1);
}

void stepper5w::rotate(const unsigned int degrees)
{
    for (unsigned int i = 0; i < (_steps_per_rev * degrees) / 360; i++) {
        step();
    };
}
