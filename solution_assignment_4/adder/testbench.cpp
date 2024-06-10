#include "testbench.h"

/**
 * constructor implmentation.
 *
 * The SC_CTOR() macro expands (amongst others) to
 * a constructor with a sc_module_name parameter.
 *
 */
testbench::testbench( sc_core::sc_module_name /* unused */ )
  : uut("uut")             // name the sub-module
  , clk("clk", 10, sc_core::SC_NS ) // clock initialisation
  , rst("rst")             // more naming
  , ch_x("x")
  , ch_y("y")
  , ch_s("s")
{
  /* ----- port bindings ----- */
  uut.clk(clk);
  uut.rst(rst);

  uut.x(ch_x);
  uut.y(ch_y);
  uut.s(ch_s);

  /* ----- process definitions ----- */
  SC_THREAD( stim );

  SC_METHOD( check );
    sensitive << ch_s;
    dont_initialize();
}


/**
 * stimulating process (SC_THREAD).
 */
void
testbench::stim()
{
  // begin with a reset phase

  rst = true;
  wait( 10, sc_core::SC_NS );
  rst = false;

  ch_x = 3; ch_y = 4;
  wait( 10, sc_core::SC_NS );

  ch_x = 7; ch_y = 0;
  wait( 10, sc_core::SC_NS );

  ch_x = 8; ch_y = 4;
  wait( 10, sc_core::SC_NS );

  ch_x = 8; ch_y = 4;
  wait( 10, sc_core::SC_NS );

  ch_x = 9; ch_y = -3;
  wait( 20, sc_core::SC_NS );

  ch_x = -1; ch_y = 3;
  wait( 5, sc_core::SC_NS );

  ch_x = 1; ch_y = 3;
  wait( 5, sc_core::SC_NS );

  ch_x = 2; ch_y = 3;
  wait( 10, sc_core::SC_NS );

  // what happens, if we skip the next line?
  // why?
  sc_core::sc_stop();
}


/**
 * checking process (SC_METHOD). validates results
 */
void
testbench::check()
{
  std::cout
    // print process name
    << sc_core::sc_get_current_process_handle().name()
    // print simulation time
    << " @ " << sc_core::sc_time_stamp()
    // print result
    << ": " << ch_x << " + " << ch_y << " = " << ch_s
    // print reset
    << " (rst:" << std::boolalpha << rst << ")"
    << " -> "
    // check result -- print output
    << ( ( !rst && ch_s != ch_x + ch_y ) ? "FAIL" : "OK" )
    << std::endl;

  // abort simulation, if condition fails
  //sc_assert( ch_s == ch_x + ch_y );

}
// :tag: (exercise1,s)
