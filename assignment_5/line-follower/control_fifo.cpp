#include "control_fifo.h"

#include <systemc>

// constructor (should be fine)
control_fifo::control_fifo( sc_core::sc_module_name /* unused */ )
: sc_core::sc_prim_channel()
, m_pending(0)
, m_written(false)
, m_read(false)
{}

// TODO: Implement all declared methods

/* ---------------------------------------------------------------
 * Methods from: sc_fifo_out_if< control_data >
 */
bool control_fifo::nb_write( control_data const & data )
{
    // non-blocking write - consumer is responsible for checking if the writing
    // was successful, and if not, for calling this method again

    /* --- check, if write is possible --- */
    if ( !num_free() )
        return false;

    /* --- TODO: do the write --- */
    // TODO: we need to call to_float() once with field = 2, once with field = 1; and then, use the returned floats in a meaningful way
    // What needs to be written?
    // we have some flags, maybe they can be used?
    // we want to write, so m_buffer is probably not needed
    // --> m_pending, m_written, m_data_written_event?
    // we would need to convert the data to floats, correct?

    // TODO: set the write flag
    // write done in the last evaluation phase
    m_written = true;

    // request call to this->update() in the next update phase
    request_update();
    return true;
}

sc_core::sc_event const & control_fifo::data_read_event() const
{
    return m_data_read_event;
}

void control_fifo::write( control_data const & data )
{
    // this is the blocking write - we busy wait until we can write
    /* --- TODO: check, if write is possible ---
     *   - otherwise wait for data_read_event
     *   - is a simple condition sufficient?
     */

    /* --- TODO: do the write --- */

    // TODO: set write flag accordingly

    request_update();
}

// additional
int control_fifo::num_free() const
{
    // 1 position free, if nothing is pending --> true, it is free
    // nothing free, otherwise --> 0 == false, so m_pending is not 0,
    // something is pending and we can't write
    return ( m_pending == 0 );
}

/* ---------------------------------------------------------------
 * Methods from: sc_fifo_in_if< float >
 */

// here, we retrieve the data from the input fifo of the channel (car_controller direction)
// and forward it to the pointer which is given
bool control_fifo::nb_read( float& data )
{
    /* --- check, if read is possible --- */
    if(!num_available())
        return false;

    /* --- TODO: perform read ... --- */

    // TODO: set flag

    // directly modifies the value which is behind the passed reference of `data`
    data = 0;

    // return `true` in case the read was successful, since this is non-blocking
    return true;
}

sc_core::sc_event const & control_fifo::data_written_event() const
{
    return m_data_written_event;
}

// this Method is currently called by car.cpp - it only knows of this channel
// as a `sc_core::sc_fifo_in<float> control`!
float control_fifo::read()
{
    /* --- TODO: check if read is possible
     *   - otherwise wait for data_written_event
     *   - is a simple condition sufficient?
     */

    /* --- TODO: perform read ... --- */

    // TODO: set flag

    // FIXME: why do we return data? of what type must it be? probably the channel interface type?
    return data;
}

int control_fifo::num_available() const
{
    return m_pending;
}

/* ---------------------------------------------------------------
 * Methods from: sc_prim_channel
 */
void control_fifo::update()
{
    /* --- notify appropriate events --- */
    // What are the appropriate events? This update method is called for both read and write.
    // --> notify the correct things based on the flags
    if (m_read) {
      m_data_read_event.notify();
    }
    if (m_written) {
      m_data_written_event.notify();
    }

    /* --- handle flags --- */
    m_read = false;
    m_read = false;
}

/* ---------------------------------------------------------------
 * numeric conversion
 */
float control_fifo::to_float( int field ) const
{
    switch(field) {
        case 2:
            return m_buffer.movement / 1000.f;
        case 1:
            return m_buffer.rotation / 100.f;
        default:
            sc_assert( false && "Error: should not be here!" );
    }
    return 0.f;
}

/* vim: set ts=4 sw=4 tw=72 et :*/
