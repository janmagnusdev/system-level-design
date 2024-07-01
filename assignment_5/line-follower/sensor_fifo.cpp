#include "sensor_fifo.h"

// constructor (should be fine)
// constructor, because class is named sensor_fifo and the method has the same name
// after constructor declaration, initialization follows after : (colon sign)
sensor_fifo::sensor_fifo( sc_core::sc_module_name /* unused */ )
: clock( "clock" )
, reset( "reset" )
, in( "input" )
, out( "output" )
/* --- initialise local channels and submodules --- */
// 13 incoming floats
, fifo_in("fifo_in", 13)
// 1 outgoing sensor_data
, fifo_out("fifo_out", 1)
{
    // process declaration
    SC_CTHREAD( forward, clock.pos() );
    reset_signal_is( reset, true );

    /* --- bind local channels and sub-modules --- */
    out(fifo_out);
    in(fifo_in);
}

void sensor_fifo::forward()
{
    while ( true ) {
        sensor_data sd;
        float current_float;

        // read movement
        // 0 is false, >= 1 is true
        // num_free == 0 -> no more writeable
        // num_available == 0 -> no more readable
        // for non-blocking, we must check ourselves if we can write/ read or not
        // however, here we are not blocking - but we check nonetheless if there is available data at all
        while (!fifo_in.num_available()) {
          // wait(); is static sensitivity - continues from here until next clock cycle, in this SC_CTHREAD case
          wait();
        }
        // FIXME: why do we need this to_byte call?
        sd.movement = to_byte(fifo_in.read());

        // read sensors

        for (unsigned int i = 0; i < NUMBER_OF_SENSORS; ++i) {
          while(!fifo_in.num_available()) {
            wait();
          }
          current_float = fifo_in.read();
          sd.sensor[i] = current_float;
        }

        // write to output fifo
        fifo_out.write(sd);
    }
}

// numeric conversion
sc_dt::sc_bv<8> sensor_fifo::to_byte( float value, unsigned field )
{
    switch(field) {
        case 12:
            return sc_dt::sc_int<8>( value * 1000.f );
        case 11: case 10: case 9: case 8:
        case  7: case  6: case 5: case 4:
        case  3: case  2: case 1: case 0:
            return sc_dt::sc_uint<8>( value );

        default:
            sc_assert( false && "Error: should not be here!" );
    }
    return sc_dt::sc_bv<8>(0);

}
