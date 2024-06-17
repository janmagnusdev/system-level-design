#ifndef FIFO_RX_UNIT_
#define FIFO_RX_UNIT_

#include <systemc.h>

class fifo_rx_unit
  /* --- add base classes --- */
{
 public:
  /* signal interface to uart_unit */

  /* export of fifo_in_if */

  /* sc_fifo_in_if< sc_bv<8> > interface methods */

  /* non-blocking read */
  virtual bool nb_read( sc_bv<8>& f)
  {
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
  }

  /* return number of available tokens */
  virtual int num_available() const
  {
    return ( m_is_empty == 0 );
  }

  /* rx_unit protocol process */
  void rx_proc()
  {
  }

  /* constructor */
  SC_CTOR( fifo_rx_unit )
    : m_is_empty(true)
  {
    /* --- bind export to channel itself --- */

    /* --- process definition(s) ---  */

  }

private:
  /* --- local member variables ---
   *  - flags
   *  - buffers
   *  - events
   */
};

#endif // FIFO_RX_UNIT_
