
#include <systemc>

#include "accumulator.h"

/**
 * constructor implementation.
 *
 * The SC_CTOR() macro expands (amongst others) to
 * a constructor with a sc_module_name parameter.
 *
 */
accumulator::accumulator( sc_core::sc_module_name /* unused */ )
 /* ---- initialiser list ---- */
  : clk("clk")               // port names
  , rst("rst")
  , din("din")
  , dout("dout")

  , m_adder("adder")         // sub-module
  , ch_y("y")                // local channel
{
  /* ----- port bindings ----- */
  m_adder.clk(clk);
  m_adder.rst(rst);

  m_adder.x(din);
  m_adder.y(ch_y);
  m_adder.s(ch_y);

  /* ----- process definitions ----- */
  SC_METHOD( forward );
    sensitive << ch_y;
    dont_initialize();
}

/* ----- process body ----- */
void
accumulator::forward()
{
  // What do we need here?
  dout = ch_y.read();
}

// :tag: (exercise1,s)
