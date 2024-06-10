#include "adder.h"

/* ----- process body ----- */
void
adder::add()
{
  while( true )
  {
    	/* ----- main loop ----- */

	/* ----- wait ----- */
    	wait( clk.posedge_event() | rst.value_changed_event());
  	/* ----- reset block ----- */
	if (rst == true) s = 0;
    	/* ----- calculation ----- */
	else if (clk.posedge()) s = x + y;
  }

} // adder::add()

// :tag: (exercise1,s)
