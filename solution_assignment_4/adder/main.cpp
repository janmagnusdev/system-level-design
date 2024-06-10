
#include "testbench.h"

/* ----- sc_main() ----- */

int sc_main( int /* argc unused */, char* /* argv unused */[] )
{

  // prepare tracing
  sc_core::sc_set_time_resolution( 1, sc_core::SC_NS );
  sc_core::sc_trace_file *tf = sc_core::sc_create_vcd_trace_file( "trace" );
  tf->set_time_unit( 1, sc_core::SC_NS );

  testbench top( "top" );

  // trace interesting signals
  sc_trace(tf, top.clk,  top.clk.name() );
  sc_trace(tf, top.rst,  top.rst.name() );

  sc_trace(tf, top.ch_x, top.ch_x.name() );
  sc_trace(tf, top.ch_y, top.ch_y.name() );
  sc_trace(tf, top.ch_s, top.ch_s.name() );

  // start simulation -- see testbench
  sc_core::sc_start();

  sc_core::sc_close_vcd_trace_file(tf);

  std::cout << "Simulation finished." << std::endl;
  return 0;
}
// :tag: (exercise1,s)
