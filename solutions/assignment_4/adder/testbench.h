#ifndef TESTBENCH_H_INCLUDED_
#define TESTBENCH_H_INCLUDED_

#include <systemc>

#include "adder.h"

SC_MODULE( testbench )
{
  /* ----- sub-module(s) ----- */
  adder uut;

  /* ----- channels ----- */
  sc_core::sc_clock        clk;
  sc_core::sc_signal<bool> rst;

  sc_core::sc_signal<int>  ch_x;
  sc_core::sc_signal<int>  ch_y;
  sc_core::sc_buffer<int>  ch_s;

  /* ----- constructor ----- */
  SC_CTOR( testbench );
  // implemented in testbench.cpp

private:
  /* ----- process(es) ----- */
  void stim();
  void check();

}; // testbench

#endif // TESTBENCH_H_INCLUDED_
// :tag: (exercise1,s)
