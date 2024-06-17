#ifndef CAR_H_
#define CAR_H_

#include <simulation.h> // Irrlicht3D engine wrapper
#include "data_types.h"

#include <systemc>

SC_MODULE( car )
{
public:
    /* DONE: ----- sensor and controller ports ----- */
    // out interface: .write()
    // in interface: .read()
    sc_core::sc_fifo_out<sensor_data> sensors;
    sc_core::sc_fifo_in<control_data> control;

    /* ----- alternate constructor declaration ---- */
    SC_HAS_PROCESS( car );
    car( sc_core::sc_module_name =
         sc_core::sc_gen_unique_name("car") );

private:
    /* ----- process ---- */
    void run_simulation();

    simulation_engine e;
};

#endif

/* vim: set ts=4 sw=4 tw=72 et :*/
