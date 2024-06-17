
#include "car_controller.h"
#include "process_data.h"

#include <systemc>

void car_controller::control_process()
{
    sensor_data sd;
    control_data cd;
    while ( true ) { // infinite loop

        /* ----- read sensor data ----- */
        sd = sensors->read(); // blocking read on sensor data

        std::cout << name() << "@" << sc_core::sc_time_stamp()
            << " : received sensor data = " << sd
            << std::endl;

        // this method takes the sensor data and writes control data
        // calculated from it into the cd pointer
        car_controller_set_control_data( &sd, &cd );

        /* ----- write control data ----- */
        control->write(cd); // blocking write on control data

    }
}
/* vim: set ts=4 sw=4 tw=72 et :*/
