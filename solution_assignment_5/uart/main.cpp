#include "uart.h"
#include "fifo_tx.h"
#include "fifo_rx_unit.h"

SC_MODULE(testbench)
{
 public:
  sc_out<bool> reset;

  sc_fifo_out< float > fifo_out;
  sc_fifo_in< sc_bv<8> > fifo_in;

  void tx_proc()
  {
   reset = true;
   wait(sc_time(100,SC_NS));
   reset = false;
   wait(sc_time(10,SC_NS));
   for (unsigned i = 0; i < 255; i++) {
     fifo_out->write(i);
     data_sent.write(i);
   }
   wait(sc_time(10000,SC_NS));
   sc_stop();
  }

  void rx_proc()
  {
     unsigned counter = 0;
     while (true) {
       sc_bv<8> data_in = fifo_in.read();
       float fexpected = data_sent.read();
       sc_bv<8> expected = int(fexpected);
       if (counter == 0)
         expected = int(fexpected * 1000);

       cout << "Received " << data_in << " == " << expected;
       if (expected == data_in)
         cout << " OK" << endl;
       else
         cout << " ERROR" << endl;

       if (counter == 12)
         counter = 0;
       else
         ++counter;
     }
  }

  SC_CTOR(testbench)
  {
    SC_THREAD(tx_proc);
    SC_THREAD(rx_proc);
  }

  sc_fifo<float> data_sent;

};

int sc_main( int /* argc unused */, char* /* argv unused */[] )
{
  sc_clock clk("clk",sc_time(10,SC_NS));
  sc_signal<bool> reset, txd, rx_en;
  sc_signal< sc_bv<8> > rx_data;

  fifo_tx tx("fifo_tx",115200);
    tx.txd(txd);

  fifo_rx_unit rx("fifo_uart_rx");
    rx.rx_ready(rx_en);
    rx.rx_data(rx_data);

  rx_unit<115200,100> rx_module("rx_module");
    rx_module.clk(clk);
    rx_module.reset(reset);
    rx_module.rxd(txd);
    rx_module.rx_en(rx_en);
    rx_module.rx_data(rx_data);

  testbench tb("tb");
    tb.reset(reset);
    tb.fifo_out(tx);
    tb.fifo_in(rx.out);

/*  sc_trace_file* file = sc_create_vcd_trace_file("trace");
  sc_trace(file,clk,"clk");
  sc_trace(file,reset,"reset");

  sc_trace(file,tx_module.baud_clk, "tx_module.baud_clk");
  sc_trace(file,tx_data, "tx_module.tx_data");
  sc_trace(file,tx_load, "tx_module.tx_load");
  sc_trace(file, busy, "tx_module.busy");
  sc_trace(file,txd,"txd");

  sc_trace(file,rx_module.baud_clk, "rx_module.baud_clk");
  sc_trace(file, rx_module.rx_en, "rx_module.rx_en");
  sc_trace(file, rx_module.rx_data, "rx_module.rx_data");
  sc_trace(file, rx_module.counter, "rx_module.counter");*/
  //sc_start(1,SC_MS);
  sc_start();
//   sc_close_vcd_trace_file(file);

  return 0;
}
/* :tag: (exercise2,s) */
