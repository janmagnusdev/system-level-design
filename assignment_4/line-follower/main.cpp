#include "testbench.h"

#include <systemc>

int sc_main( int /* argc unused */, char* /* argv unused */[] )
{
  /* ----- testbench instantiation and simulation start ----- */
  testbench t("testbench");
  sc_core::sc_start();
  std::cout << "Simulation finished." << std::endl;
  return 0;
}
