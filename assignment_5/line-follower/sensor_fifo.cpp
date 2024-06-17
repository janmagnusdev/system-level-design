#include "sensor_fifo.h"

// constructor (should be fine)
sensor_fifo::sensor_fifo( sc_core::sc_module_name /* unused */ )
: clock( "clock" )
, reset( "reset" )
, in( "input" )
, out( "output" )
/* --- initialise local channels and submodules --- */
{
    // process declaration
    SC_CTHREAD( forward, clock.pos() );
    reset_signal_is( reset, true );

    /* --- bind local channels and sub-modules --- */
}

void sensor_fifo::forward()
{
    while ( true ) {
        sensor_data sd;

        // read movement

        // read sensors

        // write to output fifo
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
