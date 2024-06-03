#ifndef ACCUMULATOR_H_INCLUDED_
#define ACCUMULATOR_H_INCLUDED_

#include <systemc>
#include "adder.h"

SC_MODULE( accumulator )
{
  /* ----- input ports ----- */

  /* ----- output ports ----- */


  /* ----- constructor ----- */
  SC_CTOR( accumulator );
  // implementation in accumulator.cpp

private:
  /* ----- process(es) ----- */
  void forward();

  /* ----- sub-module(s) ----- */

  /* ----- local channels ----- */

}; // accumulator

#endif // ACCUMULATOR_H_INCLUDED_
