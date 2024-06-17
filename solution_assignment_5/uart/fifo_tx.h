#ifndef FIFO_TX_H_
#define FIFO_TX_H_

#include <systemc.h>

#ifndef NUMBER_OF_SENSORS
#  define NUMBER_OF_SENSORS 12
#endif

class fifo_tx
  /* --- add base classes --- */
  : public sc_fifo_out_if<float>
  , public sc_channel
{
public:
  /* RS232 serial tx data line */
  sc_out<bool> txd;

  /* sc_fifo_out_if<float> interface methods */

  /* blocking write */
  void write( const float& f)
  {
    /* implementation of blocking write */
    // send start bit
    txd.write(false);
    wait(delay);

    // convert float to byte
    sc_bv<8> byte = to_byte(f);

    // send bits (LSB first)
    for (unsigned i = 0; i < 8; i++) {
      txd.write(byte[i].to_bool());
      wait(delay);
    }

    // send stop bit
    txd.write(true);
    wait(delay);
  }

  /* non-blocking write */
  virtual bool nb_write( const float& )
  {
    SC_REPORT_ERROR("fifo_tx", "Non-blocking write not implemented!");
    return false;
  }

  virtual const sc_event& data_read_event() const
  {
    // we dont have one, so we just return the default_event
    return default_event();
  }

  /* get number of free slots */
  virtual int num_free() const
  {
    // does not really make sense in this case
    SC_REPORT_WARNING("fifo_tx", "Calling num_free() on fifo_tx transactor does not make sense!");
    return 1;
  }

  /* constructor with extra parameter for baud_rate */
  fifo_tx(sc_module_name nm, unsigned baud_rate = 115200)
    : sc_module(nm)
    , txd("txd")
    , m_count(0)
  {
    // set the delay for the baud rate
    delay = sc_time(int(1000000000/baud_rate),SC_NS);
  }

  /* overridden method from sc_module, called by simulation kernel */
  void start_of_simulation()
  {
    /* make sure txd is initially true when simulation starts! */
    txd.write(true);
  }

 private:
  /* member to store delay for baud rate */
  sc_time delay;

  /* convert float to bit vector */
  sc_bv<8> to_byte(float f)
  {
    /* Sensor values and movement speed are converted differently.
       Therefore we count the converted tokens.
       First token is movement speed, next 12 token are sensor values.

       Take care, that the same order is used when writing floats to
       the FIFO transactor in module _car_!
    */
    sc_bv<8> byte;
    if (m_count == 0) // movement speed
      byte = (int)(f*1000) & 0x0FF;
    else // sensor values
      byte = (int)f & 0x0FF;

    if (m_count == NUMBER_OF_SENSORS)
      m_count = 0;
    else
      ++m_count;
     return byte;
  }

  unsigned m_count;
};

#endif // FIFO_TX_H_
/* :tag: (exercise2,s) (exercise4,s) */
