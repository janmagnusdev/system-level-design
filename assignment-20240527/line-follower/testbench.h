#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include "car.h"
#include "car_controller.h"


#include <systemc>

SC_MODULE(testbench)
{
public:
    /* ----- submodules ----- */

    /* ----- fifo channels ----- */

    /* ----- constructor ----- */
    SC_CTOR(testbench)
    /* ----- initialiser list ----- */
    {
        /* ----- port to channel binding ----- */
    }

};

#endif // TESTBENCH_H_
