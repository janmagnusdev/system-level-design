#ifndef CAR_CONTROLLER_H_
#define CAR_CONTROLLER_H_

#include "data_types.h"

#include <systemc>

SC_MODULE( car_controller )
{
    /* DONE: ----- sensor and controller ports ----- */
    sc_core::sc_fifo_in<sensor_data> sensors;
    sc_core::sc_fifo_out<control_data> control;

    /* ----- constructor ----- */
    SC_CTOR( car_controller )
    {
        /* DONE: ----- process definitions ----- */
        // make control_process adhere to wait() statements
        // SC_METHOD would be agnostic of wait() statements
        SC_THREAD(control_process);
    }

private:
    /* ----- control process ----- */
    void control_process();

};

#endif // CAR_CONTROLLER_H_

/* vim: set ts=4 sw=4 tw=72 et :*/
