#include "car.h"

#include <systemc>

car::car(sc_core::sc_module_name /* unused */)
: sc_module()
, e("scenery/")
{
    /* ----- process declaration ----- */
    SC_THREAD(run_simulation);
    set_stack_size(80000000); // increase stack size of process
}

void car::run_simulation()
{
    e.init(); // initialise simulation engine

    while ( e.run() ) { // infinite loop
        // don't do anything if car is in manual mode
        if( e.RCMode() ) {
            continue;
        }

        uint32_t lasttime = e.get_timer(); // get current time from engine

        // get sensor values from engine
        const std::vector<float>& sensor_values = e.getSensors();

        // create sensor data object
        sensor_data sd;
        sd.movement = e.get_movement();
        for ( unsigned i = 0; i < NUMBER_OF_SENSORS; i++ )
            sd.sensor[i] = sensor_values[i];

        /* ----- write sensor data ----- */
        sensors->write( sd );

        // create control data object
        control_data cd;

        /* ----- read control data ----- */
        cd = control->read();



        // set parameters in simulation engine
        e.set_movement( cd.movement );
        e.set_rotation( cd.rotation );


        // wait for some time
        wait( sc_core::sc_time(e.get_timer() - lasttime, sc_core::SC_MS) );

    }
    sc_core::sc_stop();
}

// :tag: (exercise1,s) (exercise2,s) (exercise4,s)
/* vim: set ts=4 sw=4 tw=72 et :*/
