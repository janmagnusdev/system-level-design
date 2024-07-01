#ifndef CAR_H_
#define CAR_H_

#include <simulation.h> // Irrlicht3D engine wrapper
#include "data_types.h"

#include <systemc>

SC_MODULE( car )
{
public:
    /* ----- sensor and controller ports ----- */
    sc_core::sc_fifo_out<float> sensors;
    sc_core::sc_fifo_in<float>  control;

    /* ----- alternate constructor declaration ---- */
    SC_CTOR( car )
    : sc_module()
    , e("scenery/")
    {

      /* ----- process declaration ----- */
      SC_THREAD(run_simulation);
      set_stack_size(80000000); // increase stack size of process
    }

private:
    /* ----- process ---- */
    void run_simulation();

    simulation_engine e;
};

#endif

/* vim: set ts=4 sw=4 tw=72 et :*/
