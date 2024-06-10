#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include "car.h"
#include "car_controller.h"


#include <systemc>

SC_MODULE(testbench)
{
public:
    /* ----- submodules ----- */
    car_controller cc;
    car c;

    /* ----- fifo channels ----- */
    sc_core::sc_fifo<control_data> cd;
    sc_core::sc_fifo<sensor_data>  sd;

    /* ----- constructor ----- */
    SC_CTOR(testbench)
    /* ----- initialiser list ----- */
    : cc( "car_controller" )
    , c( "car" )
    {
        /* ----- port to channel binding ----- */
        c.control(cd);
        c.sensors(sd);
        cc.control(cd);
        cc.sensors(sd);
    }

};

// :tag: (exercise1,s) (exercise2,s)
#endif // TESTBENCH_H_
