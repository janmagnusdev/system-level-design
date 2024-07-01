#include "car.h"

#include <systemc>

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


        sensors.write( e.get_movement() );
        for (unsigned i = 0; i < NUMBER_OF_SENSORS; i++)
            sensors.write( sensor_values[i] );

        std::cout << name() << "@" << sc_core::sc_time_stamp()
            << " : wrote sensors"
            << "\n";

        float cd[2];

        for ( unsigned i = 0; i < 2; i++ ) {
          while (!control.nb_read(cd[i])) {
            std::cout << name() << "@" << sc_core::sc_time_stamp()
                      << " : waiting"
                      << std::endl;
            wait(control.data_written_event());
          };
        }

        std::cout << name() << "@" << sc_core::sc_time_stamp()
            << " : received control data = "
            << "(movement=" << cd[0] << ",rotation=" << cd[1] << ")"
            << std::endl;

        // set parameters in simulation engine
        e.set_movement( cd[0] );
        e.set_rotation( cd[1] );

        // wait for some time
        wait( sc_core::sc_time(e.get_timer() - lasttime, sc_core::SC_MS) );

    }
    sc_core::sc_stop();
}

/* vim: set ts=4 sw=4 tw=72 et :*/
