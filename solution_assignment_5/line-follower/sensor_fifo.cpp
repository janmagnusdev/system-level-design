#include "sensor_fifo.h"

// constructor (should be fine)
sensor_fifo::sensor_fifo( sc_core::sc_module_name /* unused */ )
: clock( "clock" )
, reset( "reset" )
, in( "input" )
, out( "output" )
/* --- initialise local channels and submodules --- */
#if TASK == 1
, fifo_in( "fifo_in", 13  )
#else
, sig_rx_data( "sig_rx_data" )
, sig_rx_en( "sig_rx_en" )
, sig_tx_rx( "sig_tx_rx" )
// sub-modules/channels
, tx( "fifo_tx" )
, rx( "rx_unit" )
, fifo_rx( "fifo_rx" )
#endif
, fifo_out( "fifo_out", 1 )
{
    // process declaration
    SC_CTHREAD( forward, clock.pos() );
    reset_signal_is( reset, true );

    /* --- bind local channels and sub-modules --- */
#if TASK == 1
    in( fifo_in );
#else
    in( tx );
    tx.txd( sig_tx_rx );

    rx.clk( clock );
    rx.reset( reset );
    rx.rx_data( sig_rx_data );
    rx.rx_en( sig_rx_en );
    rx.rxd( sig_tx_rx );

    fifo_rx.rx_ready( sig_rx_en );
    fifo_rx.rx_data( sig_rx_data );
#endif
    out( fifo_out );
}

void sensor_fifo::forward()
{
    while ( true ) {
        sensor_data sd;

        // read movement
        // use explicit wait() calls because of synchronous CTHREAD
        // behaviour
#if TASK == 1
        while ( !fifo_in.num_available() )
            wait();
        sd.movement = to_byte( fifo_in.read() );
#else
        while ( !fifo_rx.num_available() )
            wait();
        sd.movement = fifo_rx.read();
#endif

        // read sensors
        for ( unsigned i = 0;  i < NUMBER_OF_SENSORS; i++ ) {
#if TASK == 1
            while ( !fifo_in.num_available() )
                wait();
            sd.sensor[i] = to_byte( fifo_in.read(), i );
#else
            while ( !fifo_rx.num_available() )
                wait();
            sd.sensor[i] = fifo_rx.read();
#endif
        }

        // write to output fifo
        while ( !fifo_out.nb_write(sd) )
            wait();
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
// :tag: (exercise2,s) (exercise4,s)
