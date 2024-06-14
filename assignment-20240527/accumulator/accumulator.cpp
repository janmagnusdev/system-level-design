
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
 /* ---- initialiser list (required: ports, submodules & local channels) ---- */
 :  clk("clk"),
    rst("rst"),
    din("din"),
    dout("dout"),
    m_adder("m_adder"),
    s_y_dout_channel("s_y_dout_channel")
{
  /* ----- port bindings ----- */
  m_adder.clk(clk);
  m_adder.rst(rst);
  m_adder.x(din);

  // bind channel to ports
  m_adder.y(s_y_dout_channel);
  m_adder.s(s_y_dout_channel);

  /* ----- process definitions ----- */
  SC_METHOD(forward);
  sensitive << m_adder.s;
}

/* ----- process body ----- */
void
accumulator::forward()
{
  // What do we need here?
  dout = s_y_dout_channel.read();
}

