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

    /* --- DONE: do the write --- */
    // --> m_pending, but not m_written & m_data_written_event
    // --> m_written is the flag that is used to publish m_data_written_event in the next update() cycle()


    // === what to write, anyway? the data?
    // store data in buffer so that to_float() has access to it implicitly. weird C stuff...
    m_buffer = data;

    // no need to convert to float here, we do that on read
    // float rotation_float = to_float(1);
    //float movement_float = to_float(2);

    m_pending = 2;


    // === where to write it?
    // --> this is the controller to car direction. the controller writes some data which the car should consume.
    //     so, this should be written to the other direction.
    // --> writing is basically just storing the thing in the channel, until someone tries to read it.
    //     not much else to do.


    // DONE: set the write flag
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
// only one data is read at a time - but which is first? car.cpp should have the answer to that
// movement first, rotation second
bool control_fifo::nb_read( float& data )
{
    /* --- check, if read is possible --- */
    if(!num_available())
        return false;

    /* --- perform read ... --- */

    int field_to_read;
    if (m_pending <= static_cast<unsigned int>(std::numeric_limits<int>::max()) && m_pending >= 0) {
      field_to_read = static_cast<int>(m_pending);
    } else {
      sc_assert( false && "Error: The unsigned value is too large or too small to fit in an int." );
    }

    // m_buffer is filled from write
    data = to_float(field_to_read);

    // DONE: set flag
    m_read = true;
    if (m_pending > 0 ) m_pending--;
    // DONE: request update
    request_update();

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
    return 0.f;
}

// returns true ( > 0) if m_pending
// returns false (==0) if not m_pending
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
    // --> notify the correct things based on the flags. read() and write() must set the flags correctly.
    if (m_read) {
      m_data_read_event.notify(sc_core::SC_ZERO_TIME);
    }
    if (m_written) {
      m_data_written_event.notify(sc_core::SC_ZERO_TIME);
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
