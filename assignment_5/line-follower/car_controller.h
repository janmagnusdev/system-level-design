#ifndef CAR_CONTROLLER_H_
#define CAR_CONTROLLER_H_

#include "data_types.h"

#include <systemc>

SC_MODULE( car_controller )
{
    /* ----- sensor and controller ports ----- */
    sc_core::sc_fifo_in<sensor_data> sensors;
    sc_core::sc_fifo_out<control_data> control;

    /* ----- constructor ----- */
    SC_CTOR( car_controller )
    {
        /* ----- process definitions ----- */
        SC_THREAD( control_process );
    }

private:
    /* ----- control process ----- */
    void control_process();

};

#endif // CAR_CONTROLLER_H_

/* vim: set ts=4 sw=4 tw=72 et :*/
