#ifndef TESTBENCH_H_INCLUDED_
#define TESTBENCH_H_INCLUDED_

#include <systemc.h>
#include "accumulator.h"

SC_MODULE( testbench )
{
  /* ----- sub-module(s) ----- */
  accumulator uut;

  /* ----- channels ----- */
  sc_core::sc_clock        clk;
  sc_core::sc_signal<bool> rst;

  sc_core::sc_signal<int>  ch_in;
  sc_core::sc_buffer<int>  ch_out;

  /* ----- constructor ----- */
  SC_CTOR( testbench );
  // implemented in testbench.cpp

private:
  /* ----- process(es) ----- */
  void stim();
  void check();

  // local variable for expected trace number
  size_t stim_id;

  static const int    expect_table[];
  static const size_t num_stim;

}; // testbench

#endif // TESTBENCH_H_INCLUDED_
