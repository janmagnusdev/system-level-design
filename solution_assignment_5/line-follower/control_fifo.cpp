#include "control_fifo.h"

#include <systemc>

// constructor (should be fine)
control_fifo::control_fifo( sc_core::sc_module_name /* unused */ )
: sc_core::sc_prim_channel()
, m_pending(0)
, m_written(false)
, m_read(false)
{}

/* ---------------------------------------------------------------
 * sc_fifo_out_if< control_data >
 */
bool control_fifo::nb_write( control_data const & data )
{
    /* --- check, if write is possible --- */
    if ( !num_free() )
        return false;

    /* --- do the write --- */
    m_buffer  = data;
    m_pending = 2;
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
    /* --- check, if write is possible ---
     *   - otherwise wait for data_read_event
     *   - is a simple condition sufficient?
     */
    while( !num_free() )
        wait( m_data_read_event );

    /* --- do the write --- */
    m_buffer  = data;
    m_pending = 2;
    m_written = true;

    request_update();
}

// additional
int control_fifo::num_free() const
{
    // 1 position free, if nothing is pending
    // nothing free, otherwise
    return ( m_pending == 0 );
}

/* ---------------------------------------------------------------
 * sc_fifo_in_if< float >
 */

bool control_fifo::nb_read( float& data )
{
    /* --- check, if read is possible --- */
    if(!num_available())
        return false;

    /* --- perform read ... --- */
    data   = to_float( m_pending-- );
    m_read = true;
    request_update();

    return true;
}

sc_core::sc_event const & control_fifo::data_written_event() const
{
    return m_data_written_event;
}


float control_fifo::read()
{
    /* --- check if read is possible
     *   - otherwise wait for data_written_event
     *   - is a simple condition sufficient?
     */
    while( !num_available() )
        wait( m_data_written_event );

    /* --- perform read ... --- */
    float data = to_float( m_pending-- );
    m_read = true;
    request_update();

    return data;
}

int control_fifo::num_available() const
{
    return m_pending;
}

/* ---------------------------------------------------------------
 * sc_prim_channel
 */
void control_fifo::update()
{
    /* --- notify appropriate events --- */
    if( m_read )
        m_data_read_event.notify( sc_core::SC_ZERO_TIME );
    if( m_written )
        m_data_written_event.notify( sc_core::SC_ZERO_TIME );

    /* --- handle flags --- */
    m_read    = false;
    m_written = false;

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
// :tag: (exercise2,s) (exercise4,s)
