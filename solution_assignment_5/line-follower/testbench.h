#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include "car.h"
#include "car_controller.h"

#include "sensor_fifo.h"
#include "control_fifo.h"

#include <systemc>

SC_MODULE(testbench)
{
public:
    /* ----- submodules ----- */
    car_controller cc;
    car c;

    /* ----- fifo channels ----- */
    control_fifo  cd;
    sensor_fifo   sd;

    /* ----- constructor ----- */
    SC_CTOR(testbench)
    /* ----- initialiser list ----- */
    : cc( "car_controller" )
    , c( "car" )
    , cd("cd_fifo")
    , sd("sd_fifo")
    , clock( "clock", 10, sc_core::SC_NS )
    , reset("reset")
    {
        /* ----- port to channel binding ----- */
        sd.clock( clock );
        sd.reset( reset );
        c.control(cd);
        c.sensors(sd.in);
        cc.control(cd);
        cc.sensors(sd.out);
    }

    sc_core::sc_clock        clock;
    sc_core::sc_signal<bool> reset;
};

// :tag: (exercise1,s) (exercise2,s)
#endif // TESTBENCH_H_
