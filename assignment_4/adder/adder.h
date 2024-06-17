#ifndef ADDER_H_
#define ADDER_H_

#include <systemc>

SC_MODULE( adder )
{
  /* ----- input ports ----- */
    sc_core::sc_in<bool> clk;
    sc_core::sc_in<bool> rst;
    sc_core::sc_in<int> x, y;

  /* ----- output ports ----- */
    sc_core::sc_out<int> s;

  /* ----- constructor ----- */
  SC_CTOR( adder )
  {
    /* ----- process definitions ----- */
    SC_CTHREAD(add, clk.pos());
    reset_signal_is(rst, true);
  }

private:
  /* ----- process(es) ----- */
  void add();

}; // adder

#endif // ADDER_H_
