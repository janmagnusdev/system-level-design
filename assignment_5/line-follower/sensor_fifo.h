#ifndef SENSOR_FIFO_INCLUDED_
#define SENSOR_FIFO_INCLUDED_

#include "data_types.h"

/* --- include required header files --- */

#include <systemc>

SC_MODULE(sensor_fifo)
{
    /* --- port declarations --- */
    sc_core::sc_in< bool > clock;
    sc_core::sc_in< bool > reset;

    /* --- export declarations --- */

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


}; // sc_module sensor_fifo

#endif // SENSOR_FIFO_H_INCLUDED_

/* vim: set ts=4 sw=4 tw=72 et :*/
