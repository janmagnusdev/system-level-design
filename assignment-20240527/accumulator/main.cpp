
#include <systemc>

#include "testbench.h"

/* ----- sc_main() ----- */

int sc_main( int /* argc unused */, char* /* argv unused */[] )
{

  // prepare tracing

  testbench top( "top" );

  // trace interesting signals

  // start simulation -- see testbench
  sc_start();


  std::cout << "Simulation finished." << std::endl;
  return 0;
}
