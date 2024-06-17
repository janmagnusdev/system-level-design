#ifndef UART_H_
#define UART_H_

#include <systemc.h>

/* SystemC version of rx_unit */
template <unsigned baud_rate = 115200, unsigned clock_rate_mhz = 100>
SC_MODULE(rx_unit)
{
 public:
  /* clock and reset ports */
  sc_in<bool> clk;
  sc_in<bool> reset;

  /* client side interface */
  sc_out<sc_bv<8> > rx_data;
  sc_out<bool> rx_en;
  /* RS232 rx data line */
  sc_in<bool> rxd;

  /* counter for baud rate generation */
  sc_signal< unsigned > counter;

  /* baud clock and synced flag */
  sc_signal< bool > baud_clk, synced;

  /* register to sample previous value of rxd */
  bool rxd_reg;

  /* data ready flag */
  sc_signal< bool > ready;

  /* process for baud clock generation */
  void baud_clk_gen() {
    const unsigned divider = (clock_rate_mhz * 1000000) / baud_rate;

    if (reset == 1) {
      counter.write(divider / 2);
      baud_clk = false;
      synced.write(false);
      rxd_reg = false;
      return;
    }

    // if there is a falling edge on rxd
    if (rxd_reg == true && rxd == false) {
      counter.write(divider/2); // reset counter
      synced.write(true); // set sync flag
    } else if (counter.read() == divider)
      counter.write(0);  // counter wrap around
    else
      counter.write(counter.read() + 1);

    // baud clock generation
    if (counter.read() < (divider / 2))
      baud_clk = true;
    else
      baud_clk = false;

    // register rxd
    rxd_reg = rxd.read();
  }

  /* receiver process, sensitive to baud_clk */
  void rx_proc() {
    // reset
    sc_uint<8> rx_buffer = 0;
    while (true) {
      ready.write(false);

      // wait until synced
      while (synced.read() == false)
        wait();

      // wait for start bit
      while (rxd.read() == true)
        wait(); // wait for start bit

      wait(); // wait for next bit

      // receive data (LSB first)
      for (unsigned i = 0; i < 8; i++) {
        rx_buffer[i] = rxd.read();
        wait();
      }

      // write received data to output
      rx_data.write(rx_buffer);
      ready.write(true);

      // stop bit
      wait();
    }
  }

  /* controller process, sensitive to clk */
  void ctrl_proc()
  {
    while (true) {
      // reset
      rx_en.write(false);
      wait();
      // wait for rising edge of ready
      bool ready_reg = ready.read();
      while (!((ready_reg == false) && (ready.read() == true))) {
        ready_reg = ready.read();
        wait();
      }
      rx_en.write(true); // new byte available
      wait();
    }
  }

  /* constructor */
  SC_CTOR(rx_unit)
    : rxd_reg(false)
  {
    SC_METHOD(baud_clk_gen);
    sensitive << clk.pos();
    SC_CTHREAD(rx_proc, baud_clk);
    reset_signal_is(reset, true);
    SC_CTHREAD(ctrl_proc, clk.pos());
    reset_signal_is(reset, true);
  }
};

template <unsigned baud_rate = 115200, unsigned clock_rate_mhz = 100>
SC_MODULE(tx_unit)
{
 public:
  sc_in<bool> clk;
  sc_in<bool> reset;

  sc_in<sc_bv<8> > tx_data;
  sc_in<bool> tx_load;
  sc_out<bool> busy;
  sc_out<bool> txd;

  sc_signal< sc_uint<32> > counter;
  sc_signal< bool > baud_clk;

  void baud_clk_gen() {
    const unsigned divider = (clock_rate_mhz * 1000000) / baud_rate;
    if ((reset == 1) || (counter.read() == divider)) {
      counter.write(0);
      baud_clk = false;
    } else
      counter.write(counter.read() + 1);
    if (counter.read() < (divider / 2))
      baud_clk = true;
    else
      baud_clk = false;
  }

  void tx_proc() {
    txd = 1;
    ready.write(true);
    while (true) {
      while (start.read() == false)
        wait(); // idle
      // start bit
      ready.write(false);
      txd.write(0);
      sc_bv<8> tx_buffer = tx_data.read(); // register input
      wait();
      // send data (LSB first)
      for (unsigned i = 0; i < 8; i++) {
        txd.write(tx_buffer[i].to_bool());
        wait();
      }
      // stop bit
      ready.write(true);
      txd.write(1);
      wait();
    }
  }

  void ctrl_proc()
  {
    while (true) {
      busy.write(false);
      wait();
      while (tx_load.read() == false)
        wait();
      busy.write(true);
      // wait for ready
      while (ready.read() == false)
        wait();
      bool ready_reg = ready.read();
      // set start
      start.write(true);
      // wait for falling edge ready
      while (!((ready_reg == true) && (ready.read() == false))) {
        ready_reg = ready.read();
        wait();
      }
      start = false;
      // wait for rising edge ready
      ready_reg = ready.read();
      while (!((ready_reg == false) && (ready.read() == true))) {
        ready_reg = ready.read();
        wait();
      }
    }
  }

  sc_signal< bool > start, ready;

  SC_CTOR(tx_unit)
  {
    SC_METHOD(baud_clk_gen);
    sensitive << clk.pos();
    SC_CTHREAD(tx_proc, baud_clk);
    reset_signal_is(reset, true);
    SC_CTHREAD(ctrl_proc, clk.pos());
    reset_signal_is(reset, true);
  }

};

#endif
/* :tag: (exercise2,s) (exercise4,s) */
