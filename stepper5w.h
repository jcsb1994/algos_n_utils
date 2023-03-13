/*! 
DC motors that move in discrete steps
Coil
Phase:
Unipolar vs Bipolar
Unipolar have less torque, simple to operate with transistor
X-wire
Step count

Stepper motors that can be driven with
commonly available drivers. These are: Permanent Magnet or Hybrid steppers, either
2-phase bipolar, or 4-phase unipolar

28BYJ-48
unipolar 5 wire
Phases: 4
H bridge driver

A 5-wire stepper can always be driven in unipolar mode
The common wire is connected to VCC, so when a transistor is on, will draw curr in the coil
and the 4 coil end wires must be powered in the right sequence
*/
#include "stdint.h"

#define STEPPER5W_COIL_NB   4
#define STEPPER5W_STEPS_NB  (STEPPER5W_COIL_NB*2)

class stepper5w
{
public:
    stepper5w(const unsigned int steps_per_rev) : _steps_per_rev(steps_per_rev) {}
    ~stepper5w() {}
    void rotate(const unsigned int degrees);
    void step();
    /*! \brief Pass the cb to the implementation. Must write coil values to gpios */
    void set_step_cb(void (*step_cb)(uint8_t coil_state_binary)) { _step_cb = step_cb; }

private:
    const unsigned int _steps_per_rev;
    uint8_t _step;
    void (*_step_cb)(uint8_t);
    const uint8_t _step_table[STEPPER5W_STEPS_NB] = {
        0b1000, 0b1100, 0b0100, 0b0110,
        0b0010, 0b0011, 0b0001, 0b1001
    };
};
