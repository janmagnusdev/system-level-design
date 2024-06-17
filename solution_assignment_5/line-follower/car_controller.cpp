
#include "car_controller.h"
#include "process_data.h"

#include <systemc>

void car_controller::control_process()
{
    sensor_data sd;
    control_data cd;
    while ( true ) { // infinite loop

        /* ----- read sensor data ----- */
        std::cout << name() << "@" << sc_core::sc_time_stamp()
            << " : requesting sensor data." << std::endl;

        sd = sensors->read(); // blocking read on sensor fifo port

        std::cout << name() << "@" << sc_core::sc_time_stamp()
            << " : received sensor data = " << sd
            << std::endl;

        car_controller_set_control_data( &sd, &cd );

        /* ----- write control data ----- */
        control->write(cd); // write control data to control fifo port

    }
}
// :tag: (exercise1,s) (exercise2,s) (exercise4,s)
/* vim: set ts=4 sw=4 tw=72 et :*/
