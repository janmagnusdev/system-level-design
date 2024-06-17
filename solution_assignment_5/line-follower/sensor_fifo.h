#ifndef SENSOR_FIFO_INCLUDED_
#define SENSOR_FIFO_INCLUDED_

#include "data_types.h"

/* --- include required header files --- */
#if TASK != 1
#include <uart.h>
#include <fifo_tx.h>
#include <fifo_rx_unit.h>
#endif

#include <systemc>

SC_MODULE(sensor_fifo)
{
    /* --- port declarations --- */
    sc_core::sc_in< bool > clock;
    sc_core::sc_in< bool > reset;

    /* --- export declarations --- */
    // incoming float data
    sc_core::sc_export< sc_core::sc_fifo_out_if< float > >      in;
    // outgoing sensor_data
    sc_core::sc_export< sc_core::sc_fifo_in_if< sensor_data > > out;

    /// constructor
    SC_CTOR( sensor_fifo );

    /// clocked process
    void forward();

private:
    // numeric conversion
    // field: 12    -> movement
    //        0-11  -> sensor
    sc_dt::sc_bv<8> to_byte( float, unsigned field = 12 );

    /* --- sub-module(s) and channels --- */

#if TASK == 1
    sc_core::sc_fifo< float >       fifo_in;
#else
    // signals
    sc_core::sc_signal< sc_dt::sc_bv<8> > sig_rx_data;
    sc_core::sc_signal< bool >      sig_rx_en;
    sc_core::sc_signal< bool >      sig_tx_rx;

    // sub-modules/channels
    fifo_tx                tx;
    rx_unit< 115200, 100 > rx;
    fifo_rx_unit           fifo_rx;
#endif
    sc_core::sc_fifo< sensor_data > fifo_out;

}; // sc_module sensor_fifo

#endif // SENSOR_FIFO_H_INCLUDED_

// :tag: (exercise2,s) (exercise4,s)
/* vim: set ts=4 sw=4 tw=72 et :*/
