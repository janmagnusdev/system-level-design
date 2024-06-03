#include "adder.h"

/* ----- process body ----- */
void
adder::add()
{

  s = 0;
  while( true )
  {
    wait();

    s = x + y;
  }

} // adder::add()

