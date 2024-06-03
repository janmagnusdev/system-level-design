#include "testbench.h"

/**
 * constructor implementation.
 *
 * The SC_CTOR() macro expands (amongst others) to
 * a constructor with a sc_module_name parameter.
 *
 */
testbench::testbench( sc_core::sc_module_name /* unused */ )
  : uut("uut")             // name the sub-module
  , clk("clk", 10, sc_core::SC_NS ) // clock initialisation
  , rst("rst")             // more naming
  , ch_in("din")
  , ch_out("dout")
  , stim_id(0)
{
  /* ----- port bindings ----- */
  uut.clk(clk);
  uut.rst(rst);

  uut.din(ch_in);
  uut.dout(ch_out);

  /* ----- process definitions ----- */
  SC_THREAD( stim );

  SC_METHOD( check );
    sensitive << clk.posedge_event();
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

  ch_in = 3;
  wait( 10, sc_core::SC_NS );

  ch_in = 0;
  wait( 10, sc_core::SC_NS );

  ch_in = 1;
  wait( 30, sc_core::SC_NS );

  ch_in = -1;
  wait( 20, sc_core::SC_NS );

  rst = true;
  wait( 10, sc_core::SC_NS );
  rst = false;

  ch_in = 2;
  wait( 20, sc_core::SC_NS );

  sc_stop();
}


/**
 * checking process (SC_METHOD). validates results
 */
void
testbench::check()
{
  // make sure, we don't simulate longer than our table is
  sc_assert( stim_id < num_stim && "Too many stimuli!" );

  // update expected value
  int expected = expect_table[stim_id];

  std::cout
    // print process name
    << sc_core::sc_get_current_process_handle().name()
    // print simulation time
    << " @ " << sc_core::sc_time_stamp()
    // print result
    << ": " << ch_out << " == " << expected
    // print reset
    << " (rst:" << std::boolalpha << rst << ")"
    << " -> "
    // check result -- print output
    << ( ( ch_out != expected ) ? "FAIL" : "OK" )
    << std::endl;

  // abort simulation, if condition fails
  // sc_assert( ch_out == expected );

  // increase stimulus counter
  stim_id++;

}


// global tables
const int
testbench::expect_table[]
  = { 0, 0 , 3, 3, 4, 5, 6, 5, 4, 0, 2 };

const size_t
testbench::num_stim = sizeof(expect_table)/sizeof(int);
