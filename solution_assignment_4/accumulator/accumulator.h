#ifndef ACCUMULATOR_H_INCLUDED_
#define ACCUMULATOR_H_INCLUDED_

#include <systemc>
#include "adder.h"

SC_MODULE( accumulator )
{
  /* ----- input ports ----- */
  sc_core::sc_in<bool> clk;
  sc_core::sc_in<bool> rst;

  sc_core::sc_in<int>  din;

  /* ----- output ports ----- */

  sc_core::sc_out<int> dout;

  /* ----- constructor ----- */
  SC_CTOR( accumulator );
  // implementation in accumulator.cpp

private:
  /* ----- process(es) ----- */
  void forward();

  /* ----- sub-module(s) ----- */
  adder m_adder;

  /* ----- local channels ----- */
  sc_core::sc_signal<int> ch_y;

}; // accumulator

#endif // ACCUMULATOR_H_INCLUDED_
// :tag: (exercise1,s)
