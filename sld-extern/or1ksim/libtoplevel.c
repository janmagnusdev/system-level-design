/* libtoplevel.c -- Top level simulator library source file

   Copyright (C) 1999 Damjan Lampret, lampret@opencores.org
   Copyright (C) 2008 Embecosm Limited
   Copyright (C) 2009 Stefan Wallentowitz, stefan.wallentowitz@tum.de
  
   Contributor Jeremy Bennett <jeremy.bennett@embecosm.com>
  
   This file is part of OpenRISC 1000 Architectural Simulator.
  
   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 3 of the License, or (at your option)
   any later version.
  
   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
   more details.
  
   You should have received a copy of the GNU General Public License along
   with this program.  If not, see <http://www.gnu.org/licenses/>. */

/* This program is commented throughout in a fashion suitable for processing
   with Doxygen. */


/* Autoconf and/or portability configuration */
#include "config.h"

/* System includes */
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

/* Package includes */
#include "or1ksim.h"
#include "sim-config.h"
#include "toplevel-support.h"
#include "sched.h"
#include "execute.h"
#include "pic.h"
#include "siminstance.h"

/*!Allocate and initialize a simulator instance
 *
 * Allocate an instance of the simulator and fill it with default values
 *
 * @return Pointer to the allocated instance
 */
or1ksim* or1ksim_instance()
{
	return sim_instance();
}

/*---------------------------------------------------------------------------*/
/*!Initialize the simulator.

   Allows specification of an (optional) config file and an image file. Builds
   up dummy argc/argv to pass to the existing argument parser.

   @param[in] sim          Simulator instance
   @param[in] config_file  Or1ksim configuration file name
   @param[in] image_file   The program image to execute
   @param[in] class_ptr    Pointer to a C++ class instance (for use when
                           called by C++)
   @param[in] upr          Upcall routine for reads
   @param[in] upw          Upcall routine for writes

   @return  0 on success and an error code on failure                        */
/*---------------------------------------------------------------------------*/
int
or1ksim_init ( or1ksim* sim,
		  const char *config_file,
	      const char *image_file,
	      void       *class_ptr,
	      unsigned long int (*upr) (void *class_ptr,
					unsigned long int addr,
					unsigned long int mask),
	      void (*upw) (void *class_ptr,
			   unsigned long int addr,
			   unsigned long int mask, unsigned long int wdata))
{
  int   dummy_argc;
  char *dummy_argv[4];

  /* Dummy argv array. Varies depending on whether an image file is
     specified. */
  dummy_argv[0] = "libsim";
  dummy_argv[1] = "-f";
  dummy_argv[2] = (char *) ((NULL != config_file) ? config_file : "sim.cfg");
  dummy_argv[3] = (char *) image_file;

  dummy_argc = (NULL == image_file) ? 3 : 4;

  /* Initialization copied from existing main() */
  srand (getpid ());
  init_defconfig ( sim );
  reg_config_secs ( sim );
  sim_register( sim );

  if (parse_args (sim,dummy_argc, dummy_argv))
    {
      return OR1KSIM_RC_BADINIT;
    }

  sim->config.sim.profile   = 0;	/* No profiling */
  sim->config.sim.mprofile  = 0;

  sim->config.ext.class_ptr = class_ptr;	/* SystemC linkage */
  sim->config.ext.read_up   = upr;
  sim->config.ext.write_up  = upw;

  print_config (sim);		/* Will go eventually */
  signal (SIGINT, ctrl_c);	/* Not sure we want this really */

  sim->runtime.sim.hush = 1;		/* Not sure if this is needed */
  sim->do_stats = sim->config.cpu.superscalar ||
                  sim->config.cpu.dependstats ||
                  sim->config.sim.history     ||
                  sim->config.sim.exe_log;

  sim_init ( sim );

  sim->runtime.sim.ext_int_set = 0;	/* No interrupts pending to be set */
  sim->runtime.sim.ext_int_clr = 0;	/* No interrupts pending to be cleared */

  return OR1KSIM_RC_OK;

}	/* or1ksim_init() */


/*---------------------------------------------------------------------------*/
/*!Run the simulator

   The argument is a time in seconds, which is converted to a number of
   cycles, if positive. A negative value means "run for ever".

   The semantics are that the duration for which the run may occur may be
   changed mid-run by a call to or1ksim_reset_duration(). This is to allow for
   the upcalls to generic components adding time, and reducing the time
   permitted for ISS execution before synchronization of the parent SystemC
   wrapper.

   This is over-ridden if the call was for a negative duration, which means
   run forever!

   Uses a simplified version of the old main program loop. Returns success if
   the requested number of cycles were run and an error code otherwise.

   @param[in] sim       Simulator instance
   @param[in] duration  Time to execute for (seconds)

   @return  OR1KSIM_RC_OK if we run to completion, OR1KSIM_RC_BRKPT if we hit
            a breakpoint (not clear how this can be set without CLI access)  */
/*---------------------------------------------------------------------------*/
int
or1ksim_run (or1ksim *sim, double duration)
{
  const int  num_ints = sizeof (sim->runtime.sim.ext_int_set) * 8;

  or1ksim_reset_duration (sim, duration);

  /* Loop until we have done enough cycles (or forever if we had a negative
     duration) */
  while (duration < 0.0 || (sim->runtime.sim.cycles < sim->runtime.sim.end_cycles))
    {

      long long int time_start = sim->runtime.sim.cycles;
      int i;			/* Interrupt # */

      /* Each cycle has counter of mem_cycles; this value is joined with cycles
       * at the end of the cycle; no sim originated memory accesses should be
       * performed inbetween.
       */

      sim->runtime.sim.mem_cycles = 0;

      if (cpu_clock (sim))
	{
	  return OR1KSIM_RC_BRKPT;	/* Hit a breakpoint */
	}

      sim->runtime.sim.cycles += sim->runtime.sim.mem_cycles;

      /* Take any external interrupts. Outer test is for the common case for
         efficiency. */
      if (0 != sim->runtime.sim.ext_int_set)
	{
	  for (i = 0; i < num_ints; i++)
	    {
	      if (0x1 == ((sim->runtime.sim.ext_int_set >> i) & 0x1))
		{
		  report_interrupt (sim, i);
		  sim->runtime.sim.ext_int_set &= ~(1 << i);	/* Clear req flag */
		}
	    }
	}

      /* Clear any interrupts as requested. For edge triggered interrupts this
	 will happen in the same cycle. For level triggered, it must be an
	 explicit call. */
      if (0 != sim->runtime.sim.ext_int_clr)
	{
	  for (i = 0; i < num_ints; i++)
	    {
	      /* Only clear interrupts that have been explicitly cleared */
	      if(0x1 == ((sim->runtime.sim.ext_int_clr >> i) & 0x1))
		{
		  clear_interrupt(sim, i);
		  sim->runtime.sim.ext_int_clr &= ~(1 << i); /* Clear clr flag */
		}
	    }
	}

      /* Update the scheduler queue */

      sim->scheduler.job_queue->time -= (sim->runtime.sim.cycles - time_start);

      if (sim->scheduler.job_queue->time <= 0)
	{
	  do_scheduler (sim);
	}
    }

  return  OR1KSIM_RC_OK;

}	/* or1ksim_run() */


/*---------------------------------------------------------------------------*/
/*!Reset the run-time simulation end point

  Reset the time for which the simulation should run to the specified duration
  from NOW (i.e. NOT from when the run started).

  @param[in] sim       Simulator instance
  @param[in] duration  Time to run for in seconds                            */
/*---------------------------------------------------------------------------*/
void
or1ksim_reset_duration (or1ksim *sim, double duration)
{
  sim->runtime.sim.end_cycles =
    sim->runtime.sim.cycles +
    (long long int) (duration * 1.0e12 / (double) sim->config.sim.clkcycle_ps);

}	/* or1ksim_reset_duration() */


/*---------------------------------------------------------------------------*/
/*!Return time executed so far

   Internal utility to return the time executed so far. Note that this is a
   re-entrant routine.

   @param[in] sim  Simulator instance
   @return         Time executed so far in seconds                           */
/*---------------------------------------------------------------------------*/
static double
internal_or1ksim_time ( or1ksim *sim )
{
  return (double) sim->runtime.sim.cycles * (double) sim->config.sim.clkcycle_ps /
    1.0e12;

}	// or1ksim_cycle_count()


/*---------------------------------------------------------------------------*/
/*!Mark a time point in the simulation

   Sets the internal parameter recording this point in the simulation        
   @param[in] sim  Simulator instance                                        */
/*---------------------------------------------------------------------------*/
void
or1ksim_set_time_point (or1ksim *sim)
{
  sim->runtime.sim.time_point = internal_or1ksim_time (sim);

}	/* or1ksim_set_time_point() */


/*---------------------------------------------------------------------------*/
/*!Return the time since the time point was set

  Get the value from the internal parameter
  @param[in] sim  Simulator instance                                         */
/*---------------------------------------------------------------------------*/
double
or1ksim_get_time_period (or1ksim *sim)
{
  return internal_or1ksim_time (sim) - sim->runtime.sim.time_point;

}	/* or1ksim_get_time_period() */


/*---------------------------------------------------------------------------*/
/*!Return the endianism of the model

   Note that this is a re-entrant routine.

   @param[in] sim  Simulator instance
   @return 1 if the model is little endian, 0 otherwise.                     */
/*---------------------------------------------------------------------------*/
int
or1ksim_is_le (or1ksim *sim)
{
#ifdef OR32_BIG_ENDIAN
  return 0;
#else
  return 1;
#endif

}	/* or1ksim_is_le() */


/*---------------------------------------------------------------------------*/
/*!Return the clock rate

   Value is part of the configuration

   @param[in] sim  Simulator instance
   @return  Clock rate in Hz.                                                */
/*---------------------------------------------------------------------------*/
unsigned long int
or1ksim_clock_rate (or1ksim *sim)
{
  return (unsigned long int) (1000000000000ULL /
			      (unsigned long long int) (sim->config.sim.
							clkcycle_ps));
}	/* or1ksim_clock_rate() */


/*---------------------------------------------------------------------------*/
/*!Trigger an edge triggered interrupt

   This function is appropriate for edge triggered interrupts, which are taken
   and then immediately cleared.

   @note There is no check that the specified interrupt number is reasonable
   (i.e. <= 31).

   @param[in] sim  Simulator instance
   @param[in] i  The interrupt number                                        */
/*---------------------------------------------------------------------------*/
void
or1ksim_interrupt (or1ksim *sim, int i)
{
  if (!sim->config.pic.edge_trigger)
    {
      fprintf (stderr, "Warning: or1ksim_interrupt should not be used for "
	       "edge triggered interrupts. Ignored\n");
    }
  else
    {
      sim->runtime.sim.ext_int_set |= 1 << i;	// Better not be > 31!
      sim->runtime.sim.ext_int_clr |= 1 << i;	// Better not be > 31!
    }
}	/* or1ksim_interrupt() */


/*---------------------------------------------------------------------------*/
/*!Set a level triggered interrupt

   This function is appropriate for level triggered interrupts, which must be
   explicitly cleared in a separate call.

   @note There is no check that the specified interrupt number is reasonable
   (i.e. <= 31).

   @param[in] sim  Simulator instance
   @param[in] i  The interrupt number to set                                 */
/*---------------------------------------------------------------------------*/
void
or1ksim_interrupt_set (or1ksim *sim, int i)
{
  if (sim->config.pic.edge_trigger)
    {
      fprintf (stderr, "Warning: or1ksim_interrupt_set should not be used for "
	       "level triggered interrupts. Ignored\n");
    }
  else
    {
      sim->runtime.sim.ext_int_set |= 1 << i;	// Better not be > 31!
    }
}	/* or1ksim_interrupt() */


/*---------------------------------------------------------------------------*/
/*!Clear a level triggered interrupt

   This function is appropriate for level triggered interrupts, which must be
   explicitly set first in a separate call.

   @note There is no check that the specified interrupt number is reasonable
   (i.e. <= 31).

   @param[in] sim  Simulator instance
   @param[in] i  The interrupt number to clear                               */
/*---------------------------------------------------------------------------*/
void
or1ksim_interrupt_clear (or1ksim *sim, int i)
{
  if (sim->config.pic.edge_trigger)
    {
      fprintf (stderr, "Warning: or1ksim_interrupt_clear should not be used "
	       "for level triggered interrupts. Ignored\n");
    }
  else
    {
      sim->runtime.sim.ext_int_clr |= 1 << i;	// Better not be > 31!
    }
}	/* or1ksim_interrupt() */
