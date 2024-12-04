#include "stepper5w.h"

void stepper5w::step(const bool dir)
{
    if (_step_cb == 0) {
        return;
    }
    _step_cb(_step_table[_step]);
    _step+= dir ? 1 : -1;
    _step &= (STEPPER5W_STEPS_NB-1); // Warning: works while steps_nb is a pwr of 2
}

void stepper5w::rotate(const bool dir, const unsigned int degrees)
{
    _remaining_steps = (_steps_per_rev * degrees) / 360;
    _dir = dir;
}

 bool stepper5w::run()
{
    if(_remaining_steps > 0) {
        step(_dir);
        _remaining_steps--;
    }

    return _remaining_steps ? false : true;
}