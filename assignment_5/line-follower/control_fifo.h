#ifndef CONTROL_FIFO_INCLUDED_
#define CONTROL_FIFO_INCLUDED_

#include "data_types.h"

#include <systemc>


struct control_fifo
/* inherit from proper interfaces */
/* inherit from correct channel base class */
, public sc_core::sc_prim_channel
{
    // constructor
    control_fifo( sc_core::sc_module_name =
                  sc_core::sc_gen_unique_name("control_fifo") );

    /* ---  sc_fifo_out_if< control_data > --- */

    // non-blocking
    virtual bool nb_write( control_data const & );
    virtual sc_core::sc_event const & data_read_event() const;

    // blocking
    virtual void write( control_data const & );

    // additional
    virtual int num_free() const;


    /* ---  sc_fifo_in_if< float > --- */

    // non-blocking
    virtual bool nb_read( float& );
    virtual sc_core::sc_event const & data_written_event() const;

    // blocking
    virtual void read( float& val ) { val = read(); }
    virtual float read();

    // additional
    virtual int num_available() const;

private:
    /* --- sc_prim_channel --- */
    virtual const char* kind() const { return "control_fifo"; }
    virtual void update();

    // numeric conversion
    // field : 2 -> movement
    //         1 -> rotation
    float to_float( int field ) const;

    /* --- members ---- */

    // buffer for incoming tokens
    control_data m_buffer;

    // status flags
    unsigned     m_pending; // floats, not yet written
    // 0 -> accept next control_data
    // 1 -> one more float available
    // 2 -> two floats available

    bool m_written; // write done in last evaluation period?
    bool m_read;    // read  done in last evaluation period?

    // local events
    sc_core::sc_event m_data_written_event;
    sc_core::sc_event m_data_read_event;

}; // class control_fifo

#endif // CONTROL_FIFO_H_INCLUDED_

/* vim: set ts=4 sw=4 tw=72 et :*/
