#ifndef FIFO_RX_UNIT_
#define FIFO_RX_UNIT_

#include <systemc.h>

class fifo_rx_unit
  /* --- add base classes --- */
  : public sc_fifo_in_if< sc_bv<8> >
  , public sc_module
{
 public:
  /* signal interface to uart_unit */
  sc_in<bool>       rx_ready;
  sc_in< sc_bv<8> > rx_data;

  /* export of fifo_in_if */
  sc_export< sc_fifo_in_if< sc_bv<8> > > out;

  /* sc_fifo_in_if< sc_bv<8> > interface methods */

  /* non-blocking read */
  virtual bool nb_read( sc_bv<8>& f)
  {
    if (m_is_empty)
      return false;

    f = m_buffer;
    m_is_empty = true;
    return true;
  }

  /* get the data written event */
  virtual const sc_event& data_written_event() const
  {
    return m_written_event;
  }

  /* alternative blocking read */
  virtual void read(sc_bv<8>& d)
  {
    d = read();
  }

  /* blocking read */
  virtual sc_bv<8> read()
  {
    while( m_is_empty )
      wait( m_written_event );

    m_is_empty = true;
    return m_buffer;
  }

  /* return number of available tokens */
  virtual int num_available() const
  {
    return ( m_is_empty == 0 );
  }

  /* rx_unit protocol process */
  void rx_proc()
  {
    //if (rx_ready == 1)
    {
      if (m_is_empty)
      {
        m_buffer   = rx_data;
        m_is_empty = false;
        m_written_event.notify();
      } else
      {
        SC_REPORT_WARNING("fifo_uart_rx",
          "Inner FIFO full. Discarding received data!");
      }
    }
  }

  /* constructor */
  SC_CTOR( fifo_rx_unit )
    : m_is_empty(true)
  {
    /* --- bind export to channel itself --- */
    out(*this); // bind export to the channel itself

    /* --- process definition(s) ---  */
    SC_METHOD(rx_proc);
    sensitive << rx_ready.pos();
    dont_initialize();

  }

private:
  /* --- local member variables ---
   *  - flags
   *  - buffers
   *  - events
   */
  bool     m_is_empty;      // remember if buffer is empty
  sc_bv<8> m_buffer;        // the buffer itself
  sc_event m_written_event; // data_written event
};

#endif // FIFO_RX_UNIT_
/* :tag: (exercise2,s) (exercise4,s) */
