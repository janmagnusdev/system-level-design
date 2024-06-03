/* except.c -- Simulation of OR1K exceptions

   Copyright (C) 1999 Damjan Lampret, lampret@opencores.org
   Copyright (C) 2008 Embecosm Limited
   Copyright (C) 2009 Stefan Wallentowitz, stefan.wallentowitz@tum.de

   Contributor Jeremy Bennett <jeremy.bennett@embecosm.com>

   This file is part of Or1ksim, the OpenRISC 1000 Architectural Simulator.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the Free
   Software Foundation; either version 3 of the License, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
   more details.

   You should have received a copy of the GNU General Public License along
   with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* This program is commented throughout in a fashion suitable for processing
   with Doxygen. */


/* Autoconf and/or portability configuration */
#include "config.h"

/* Package includes */
#include "except.h"
#include "sim-config.h"
#include "arch.h"
#include "debug.h"
#include "spr-defs.h"
#include "execute.h"
#include "debug-unit.h"

#if DYNAMIC_EXECUTION
#include "sched.h"
#include "op-support.h"
#endif

extern void op_join_mem_cycles(void);

/* Asserts OR1K exception. */
/* WARNING: Don't expect except_handle to return.  Sometimes it _may_ return at
 * other times it may not. */
void
except_handle (or1ksim *sim,oraddr_t except, oraddr_t ea)
{
  oraddr_t except_vector;

  if (debug_ignore_exception (sim,except))
    return;

#if !(DYNAMIC_EXECUTION)
  /* In the dynamic recompiler, this function never returns, so this is not
   * needed.  Ofcourse we could set it anyway, but then all code that checks
   * this variable would break, since it is never reset */
  sim->except_pending = 1;
#endif

  except_vector =
    except + (sim->cpu_state.sprs[SPR_SR] & SPR_SR_EPH ? 0xf0000000 : 0x00000000);

#if !(DYNAMIC_EXECUTION)
  sim->pcnext = except_vector;
#endif

  sim->cpu_state.sprs[SPR_EEAR_BASE] = ea;
  sim->cpu_state.sprs[SPR_ESR_BASE] = sim->cpu_state.sprs[SPR_SR];

  sim->cpu_state.sprs[SPR_SR] &= ~SPR_SR_OVE;	/* Disable overflow flag exception. */

  sim->cpu_state.sprs[SPR_SR] |= SPR_SR_SM;	/* SUPV mode */
  sim->cpu_state.sprs[SPR_SR] &= ~(SPR_SR_IEE | SPR_SR_TEE);	/* Disable interrupts. */

  /* Address translation is always disabled when starting exception. */
  sim->cpu_state.sprs[SPR_SR] &= ~SPR_SR_DME;

#if DYNAMIC_EXECUTION
  /* If we were called from do_scheduler and there were more jobs scheduled to
   * run after this, they won't run unless the following call is made since this
   * function never returns.  (If we weren't called from do_scheduler, then the
   * job at the head of the queue will still have some time remaining) */
  if (scheduler.job_queue->time <= 0)
    do_scheduler ();
#endif

  switch (except)
    {
      /* EPCR is irrelevent */
    case EXCEPT_RESET:
      break;
      /* EPCR is loaded with address of instruction that caused the exception */
    case EXCEPT_ITLBMISS:
    case EXCEPT_IPF:
      sim->cpu_state.sprs[SPR_EPCR_BASE] = ea - (sim->cpu_state.delay_insn ? 4 : 0);
#if DYNAMIC_EXECUTION
      op_join_mem_cycles ();
#endif
      break;
    case EXCEPT_BUSERR:
    case EXCEPT_DPF:
    case EXCEPT_ALIGN:
    case EXCEPT_ILLEGAL:
    case EXCEPT_DTLBMISS:
    case EXCEPT_RANGE:
    case EXCEPT_TRAP:
      /* All these exceptions happen during a simulated instruction */
#if DYNAMIC_EXECUTION
      /* Since these exceptions happen during a simulated instruction and this
       * function jumps out to the exception vector the scheduler would never have
       * a chance to run, therefore run it now */
      run_sched_out_of_line ();
#endif
      sim->cpu_state.sprs[SPR_EPCR_BASE] =
	sim->cpu_state.pc - (sim->cpu_state.delay_insn ? 4 : 0);
      break;
      /* EPCR is loaded with address of next not-yet-executed instruction */
    case EXCEPT_SYSCALL:
      sim->cpu_state.sprs[SPR_EPCR_BASE] =
	(sim->cpu_state.pc + 4) - (sim->cpu_state.delay_insn ? 4 : 0);
      break;
      /* These exceptions happen AFTER (or before) an instruction has been
       * simulated, therefore the pc already points to the *next* instruction */
    case EXCEPT_TICK:
    case EXCEPT_INT:
      sim->cpu_state.sprs[SPR_EPCR_BASE] =
	sim->cpu_state.pc - (sim->cpu_state.delay_insn ? 4 : 0);
#if !(DYNAMIC_EXECUTION)
      /* If we don't update the pc now, then it will only happen *after* the next
       * instruction (There would be serious problems if the next instruction just
       * happens to be a branch), when it should happen NOW. */
      sim->cpu_state.pc = sim->pcnext;
      sim->pcnext += 4;
#endif
      break;
    }

  /* Address trnaslation is here because run_sched_out_of_line calls
   * eval_insn_direct which checks out the immu for the address translation but
   * if it would be disabled above then there would be not much point... */
  sim->cpu_state.sprs[SPR_SR] &= ~SPR_SR_IME;

  /* Complex/simple execution strictly don't need this because of the
   * next_delay_insn thingy but in the dynamic execution modell that doesn't
   * exist and thus sim->cpu_state.delay_insn would stick in the exception handler
   * causeing grief if the first instruction of the exception handler is also in
   * the delay slot of the previous instruction */
  sim->cpu_state.delay_insn = 0;

#if DYNAMIC_EXECUTION
  do_jump (except_vector);
#endif
}
