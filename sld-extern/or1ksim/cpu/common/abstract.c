/* abstract.c -- Abstract entities

   Copyright (C) 1999 Damjan Lampret, lampret@opencores.org
   Copyright (C) 2005 Gy�rgy `nog' Jeney, nog@sdf.lonestar.org
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

/* Abstract memory and routines that go with this. I need to add all sorts of
   other abstract entities. Currently we have only memory. */


/* Autoconf and/or portability configuration */
#include "config.h"
#include "port.h"

/* System includes */
#include <stdlib.h>

/* Package includes */
#include "abstract.h"
#include "except.h"
#include "support/profile.h"
#include "debug-unit.h"
#include "icache-model.h"
#include "dcache-model.h"
#include "labels.h"
#include "opcode/or32.h"
#include "dmmu.h"
#include "immu.h"
#include "siminstance.h"

#if DYNAMIC_EXECUTION
#include "dyn-rec.h"
#endif

/* Forward declarations */
static uint32_t eval_mem_32_inv (or1ksim *sim,oraddr_t, void *);
static uint16_t eval_mem_16_inv (or1ksim *sim,oraddr_t, void *);
static uint8_t  eval_mem_8_inv (or1ksim *sim,oraddr_t, void *);
static uint32_t eval_mem_32_inv_direct (or1ksim *sim, oraddr_t, void *);
static uint16_t eval_mem_16_inv_direct (or1ksim *sim, oraddr_t, void *);
static uint8_t  eval_mem_8_inv_direct (or1ksim *sim, oraddr_t, void *);
static void     set_mem_32_inv (or1ksim *sim,oraddr_t, uint32_t, void *);
static void     set_mem_16_inv (or1ksim *sim,oraddr_t, uint16_t, void *);
static void     set_mem_8_inv (or1ksim *sim,oraddr_t, uint8_t, void *);
static void     set_mem_32_inv_direct (or1ksim *sim, oraddr_t, uint32_t, void *);
static void     set_mem_16_inv_direct (or1ksim *sim, oraddr_t, uint16_t, void *);
static void     set_mem_8_inv_direct (or1ksim *sim, oraddr_t, uint8_t, void *);

/* Calculates bit mask to fit the data */
static unsigned int
bit_mask (uint32_t data)
{
  int i = 0;
  data--;
  while (data >> i)
    data |= 1 << i++;
  return data;
}

/* Register read and write function for a memory area.
   addr is inside the area, if addr & addr_mask == addr_compare
   (used also by peripheral devices like 16450 UART etc.) */
static struct dev_memarea *
register_memoryarea_mask (or1ksim* sim, oraddr_t addr_mask,
			  oraddr_t addr_compare,
			  uint32_t size, unsigned mc_dev)
{
  struct dev_memarea **pptmp;
  unsigned int size_mask = bit_mask (size);
  int found_error = 0;
  addr_compare &= addr_mask;

  /* Go to the end of the list. */
  for (pptmp = &sim->dev_list; *pptmp; pptmp = &(*pptmp)->next)
    if (((addr_compare >= (*pptmp)->addr_compare) &&
	 (addr_compare < (*pptmp)->addr_compare + (*pptmp)->size)) ||
	((addr_compare + size > (*pptmp)->addr_compare) &&
	 (addr_compare < (*pptmp)->addr_compare + (*pptmp)->size)))
      {
	if (!found_error)
	  {
	    fprintf (stderr, "ERROR: Overlapping memory area(s):\n");
	    fprintf (stderr,
		     "\taddr & %" PRIxADDR " == %" PRIxADDR " to %" PRIxADDR
		     ", size %08" PRIx32 "\n", addr_mask, addr_compare,
		     addr_compare | bit_mask (size), size);
	  }
	found_error = 1;
	fprintf (stderr,
		 "and\taddr & %" PRIxADDR " == %" PRIxADDR " to %" PRIxADDR
		 ", size %08" PRIx32 "\n", (*pptmp)->addr_mask,
		 (*pptmp)->addr_compare,
		 (*pptmp)->addr_compare | (*pptmp)->size_mask,
		 (*pptmp)->size);
      }

  if (found_error)
    exit (-1);

  sim->cur_area = *pptmp =
    (struct dev_memarea *) malloc (sizeof (struct dev_memarea));

  if (mc_dev)
    sim->mc_area = *pptmp;

  (*pptmp)->addr_mask = addr_mask;
  (*pptmp)->addr_compare = addr_compare;
  (*pptmp)->size = size;
  (*pptmp)->size_mask = size_mask;
  (*pptmp)->log = NULL;
  (*pptmp)->valid = 1;
  (*pptmp)->next = NULL;

  return *pptmp;
}

/* Register read and write function for a memory area.
   Memory areas should be aligned. Memory area is rounded up to
   fit the nearest 2^n aligment.
   (used also by peripheral devices like 16450 UART etc.)
   If mc_dev is 1, this device will be checked first for a match
   and will be accessed in case of overlaping memory areas.
   Only one device can have this set to 1 (used for memory controller) */
struct dev_memarea *
reg_mem_area (or1ksim *sim, oraddr_t addr, uint32_t size, unsigned mc_dev,
	      struct mem_ops *ops)
{
  unsigned int size_mask = bit_mask (size);
  unsigned int addr_mask = ~size_mask;
  struct dev_memarea *mem;

  mem = register_memoryarea_mask (sim, addr_mask, addr & addr_mask, size_mask + 1,
				  mc_dev);

  memcpy (&mem->ops, ops, sizeof (struct mem_ops));
  memcpy (&mem->direct_ops, ops, sizeof (struct mem_ops));

  if (!ops->readfunc32)
    {
      mem->ops.readfunc32 = eval_mem_32_inv;
      mem->direct_ops.readfunc32 = eval_mem_32_inv_direct;
      mem->direct_ops.read_dat32 = mem;
    }
  if (!ops->readfunc16)
    {
      mem->ops.readfunc16 = eval_mem_16_inv;
      mem->direct_ops.readfunc16 = eval_mem_16_inv_direct;
      mem->direct_ops.read_dat16 = mem;
    }
  if (!ops->readfunc8)
    {
      mem->ops.readfunc8 = eval_mem_8_inv;
      mem->direct_ops.readfunc8 = eval_mem_8_inv_direct;
      mem->direct_ops.read_dat8 = mem;
    }

  if (!ops->writefunc32)
    {
      mem->ops.writefunc32 = set_mem_32_inv;
      mem->direct_ops.writefunc32 = set_mem_32_inv_direct;
      mem->direct_ops.write_dat32 = mem;
    }
  if (!ops->writefunc16)
    {
      mem->ops.writefunc16 = set_mem_16_inv;
      mem->direct_ops.writefunc16 = set_mem_16_inv_direct;
      mem->direct_ops.write_dat16 = mem;
    }
  if (!ops->writefunc8)
    {
      mem->ops.writefunc8 = set_mem_8_inv;
      mem->direct_ops.writefunc8 = set_mem_8_inv_direct;
      mem->direct_ops.write_dat8 = mem;
    }

  if (!ops->writeprog8)
    {
      mem->ops.writeprog8 = mem->ops.writefunc8;
      mem->ops.writeprog8_dat = mem->ops.write_dat8;
    }

  if (!ops->writeprog32)
    {
      mem->ops.writeprog32 = mem->ops.writefunc32;
      mem->ops.writeprog32_dat = mem->ops.write_dat32;
    }

  if (ops->log)
    {
      if (!(mem->log = fopen (ops->log, "w")))
	PRINTF ("ERR: Unable to open %s to log memory acesses to\n",
		ops->log);
    }

  return mem;
}

/* Check if access is to registered area of memory. */
struct dev_memarea *
verify_memoryarea (or1ksim* sim, oraddr_t addr)
{
  struct dev_memarea *ptmp;

  /* Check memory controller space first */
  if (sim->mc_area
      && (addr & sim->mc_area->addr_mask) ==
      (sim->mc_area->addr_compare & sim->mc_area->addr_mask))
    return sim->cur_area = sim->mc_area;

  /* Check cached value */
  if (sim->cur_area
      && (addr & sim->cur_area->addr_mask) ==
      (sim->cur_area->addr_compare & sim->cur_area->addr_mask))
    return sim->cur_area;

  /* When mc is enabled, we must check valid also, otherwise we assume it is
     nonzero */
  /* Check list of registered devices. */
  for (ptmp = sim->dev_list; ptmp; ptmp = ptmp->next)
    if ((addr & ptmp->addr_mask) == (ptmp->addr_compare & ptmp->addr_mask)
	&& ptmp->valid)
      return sim->cur_area = ptmp;
  return sim->cur_area = NULL;
}

/* Sets the valid bit (Used only by memory controllers) */
void
set_mem_valid (struct dev_memarea *mem, int valid)
{
  mem->valid = valid;
}

/* Adjusts the read and write delays for the memory area pointed to by mem. */
void
adjust_rw_delay (struct dev_memarea *mem, int delayr, int delayw)
{
  mem->ops.delayr = delayr;
  mem->ops.delayw = delayw;
}

static uint8_t
eval_mem_8_inv (or1ksim *sim,oraddr_t memaddr, void *dat)
{
  except_handle (sim,EXCEPT_BUSERR, sim->cur_vadd);
  return 0;
}

static uint16_t
eval_mem_16_inv (or1ksim *sim,oraddr_t memaddr, void *dat)
{
  except_handle (sim,EXCEPT_BUSERR, sim->cur_vadd);
  return 0;
}

static uint32_t
eval_mem_32_inv (or1ksim *sim,oraddr_t memaddr, void *dat)
{
  except_handle (sim,EXCEPT_BUSERR, sim->cur_vadd);
  return 0;
}

static void
set_mem_8_inv (or1ksim *sim,oraddr_t memaddr, uint8_t val, void *dat)
{
  except_handle (sim,EXCEPT_BUSERR, sim->cur_vadd);
}

static void
set_mem_16_inv (or1ksim *sim,oraddr_t memaddr, uint16_t val, void *dat)
{
  except_handle (sim,EXCEPT_BUSERR, sim->cur_vadd);
}

static void
set_mem_32_inv (or1ksim *sim,oraddr_t memaddr, uint32_t val, void *dat)
{
  except_handle (sim,EXCEPT_BUSERR, sim->cur_vadd);
}

uint8_t
eval_mem_8_inv_direct (or1ksim *sim, oraddr_t memaddr, void *dat)
{
  struct dev_memarea *mem = dat;

  PRINTF ("ERROR: Invalid 8-bit direct read from memory %" PRIxADDR "\n",
	  mem->addr_compare | memaddr);
  return 0;
}

uint16_t
eval_mem_16_inv_direct (or1ksim *sim, oraddr_t memaddr, void *dat)
{
  struct dev_memarea *mem = dat;

  PRINTF ("ERROR: Invalid 16-bit direct read from memory %" PRIxADDR "\n",
	  mem->addr_compare | memaddr);
  return 0;
}

uint32_t
eval_mem_32_inv_direct (or1ksim *sim, oraddr_t memaddr, void *dat)
{
  struct dev_memarea *mem = dat;

  PRINTF ("ERROR: Invalid 32-bit direct read from memory %" PRIxADDR "\n",
	  mem->addr_compare | memaddr);
  return 0;
}

void
set_mem_8_inv_direct (or1ksim *sim, oraddr_t memaddr, uint8_t val, void *dat)
{
  struct dev_memarea *mem = dat;

  PRINTF ("ERROR: Invalid 32-bit direct write to memory %" PRIxADDR "\n",
	  mem->addr_compare | memaddr);
}

void
set_mem_16_inv_direct (or1ksim *sim, oraddr_t memaddr, uint16_t val, void *dat)
{
  struct dev_memarea *mem = dat;

  PRINTF ("ERROR: Invalid 16-bit direct write to memory %" PRIxADDR "\n",
	  mem->addr_compare | memaddr);
}

void
set_mem_32_inv_direct (or1ksim *sim, oraddr_t memaddr, uint32_t val, void *dat)
{
  struct dev_memarea *mem = dat;

  PRINTF ("ERROR: Invalid 32-bit direct write to memory %" PRIxADDR "\n",
	  mem->addr_compare | memaddr);
}

/* For cpu accesses
 *
 * NOTE: This function _is_ only called from eval_mem32 below and
 * {i,d}c_simulate_read.  _Don't_ call it from anywere else.
 */
uint32_t
evalsim_mem32 (or1ksim *sim,oraddr_t memaddr, oraddr_t vaddr)
{
  struct dev_memarea *mem;

  if ((mem = verify_memoryarea (sim, memaddr)))
    {
      sim->runtime.sim.mem_cycles += mem->ops.delayr;
      return mem->ops.readfunc32 (sim, memaddr & mem->size_mask,
				  mem->ops.read_dat32);
    }
  else
    {
      PRINTF ("EXCEPTION: read out of memory (32-bit access to %" PRIxADDR
	      ")\n", memaddr);
      except_handle (sim,EXCEPT_BUSERR, vaddr);
    }

  return 0;
}

/* For cpu accesses
 *
 * NOTE: This function _is_ only called from eval_mem16 below and
 * {i,d}c_simulate_read.  _Don't_ call it from anywere else.
 */
uint16_t
evalsim_mem16 (or1ksim *sim,oraddr_t memaddr, oraddr_t vaddr)
{
  struct dev_memarea *mem;

  if ((mem = verify_memoryarea (sim, memaddr)))
    {
      sim->runtime.sim.mem_cycles += mem->ops.delayr;
      return mem->ops.readfunc16 (sim, memaddr & mem->size_mask,
				  mem->ops.read_dat16);
    }
  else
    {
      PRINTF ("EXCEPTION: read out of memory (16-bit access to %" PRIxADDR
	      ")\n", memaddr);
      except_handle (sim,EXCEPT_BUSERR, vaddr);
    }

  return 0;
}

/* For cpu accesses
 *
 * NOTE: This function _is_ only called from eval_mem8 below and
 * {i,d}c_simulate_read.  _Don't_ call it from anywere else.
 */
uint8_t
evalsim_mem8 (or1ksim *sim,oraddr_t memaddr, oraddr_t vaddr)
{
  struct dev_memarea *mem;

  if ((mem = verify_memoryarea (sim, memaddr)))
    {
      sim->runtime.sim.mem_cycles += mem->ops.delayr;
      return mem->ops.readfunc8 (sim , memaddr & mem->size_mask,
				 mem->ops.read_dat8);
    }
  else
    {
      PRINTF ("EXCEPTION: read out of memory (8-bit access to %" PRIxADDR
	      ")\n", memaddr);
      except_handle (sim,EXCEPT_BUSERR, vaddr);
    }

  return 0;
}

/* Returns 32-bit values from mem array. Big endian version.
 *
 * STATISTICS OK (only used for cpu_access, that is architectural access)
 */
uint32_t
eval_mem32 (or1ksim *sim,oraddr_t memaddr, int *breakpoint)
{
  uint32_t temp;
  oraddr_t phys_memaddr;

  if (sim->config.sim.mprofile)
    mprofile (sim,memaddr, MPROF_32 | MPROF_READ);

  if (memaddr & 3)
    {
      except_handle (sim,EXCEPT_ALIGN, memaddr);
      return 0;
    }

  if (sim->config.debug.enabled)
    *breakpoint += check_debug_unit (sim,DebugLoadAddress, memaddr);	/* 28/05/01 CZ */

  phys_memaddr = dmmu_translate (sim, memaddr, 0);
  if (sim->except_pending)
    return 0;

  if (sim->config.dc.enabled)
    temp = dc_simulate_read (sim,phys_memaddr, memaddr, 4);
  else
    temp = evalsim_mem32 (sim,phys_memaddr, memaddr);

  if (sim->config.debug.enabled)
    *breakpoint += check_debug_unit (sim,DebugLoadData, temp);	/* MM170901 */

  return temp;
}

/* for simulator accesses, the ones that cpu wouldn't do
 *
 * STATISTICS OK
 */
uint32_t
eval_direct32 (or1ksim *sim,oraddr_t memaddr, int through_mmu, int through_dc)
{
  oraddr_t phys_memaddr;
  struct dev_memarea *mem;

  if (memaddr & 3)
    {
      PRINTF ("%s:%d %s(): ERR unaligned access\n", __FILE__, __LINE__,
	      __FUNCTION__);
      return 0;
    }

  phys_memaddr = memaddr;

  if (through_mmu)
    phys_memaddr = peek_into_dtlb (sim, memaddr, 0, through_dc);

  if (through_dc)
    return dc_simulate_read (sim,phys_memaddr, memaddr, 4);
  else
    {
      if ((mem = verify_memoryarea (sim, phys_memaddr)))
	return mem->direct_ops.readfunc32 (sim, phys_memaddr & mem->size_mask,
					   mem->direct_ops.read_dat32);
      else
	PRINTF ("ERR: 32-bit read out of memory area: %" PRIxADDR
		" (physical: %" PRIxADDR ")\n", memaddr, phys_memaddr);
    }

  return 0;
}


/* Returns 32-bit values from mem array. Big endian version.
 *
 * STATISTICS OK (only used for cpu_access, that is architectural access)
 */
uint32_t
eval_insn (or1ksim *sim,oraddr_t memaddr, int *breakpoint)
{
  uint32_t temp;
  oraddr_t phys_memaddr;

  if (sim->config.sim.mprofile)
    mprofile (sim,memaddr, MPROF_32 | MPROF_FETCH);

  phys_memaddr = memaddr;
#if !(DYNAMIC_EXECUTION)
  phys_memaddr = immu_translate (sim, memaddr);

  if (sim->except_pending)
    return 0;
#endif

  if (sim->config.debug.enabled)
    *breakpoint += check_debug_unit (sim,DebugInstructionFetch, memaddr);

  if ((NULL != sim->ic_state) && sim->ic_state->enabled)
    temp = ic_simulate_fetch (sim, phys_memaddr, memaddr);
  else
    temp = evalsim_mem32 (sim,phys_memaddr, memaddr);

  if (sim->config.debug.enabled)
    *breakpoint += check_debug_unit (sim,DebugLoadData, temp);
  return temp;
}

/* Returns 16-bit values from mem array. Big endian version.
 *
 * STATISTICS OK (only used for cpu_access, that is architectural access)
 */
uint16_t
eval_mem16 (or1ksim *sim,oraddr_t memaddr, int *breakpoint)
{
  uint16_t temp;
  oraddr_t phys_memaddr;

  if (sim->config.sim.mprofile)
    mprofile (sim,memaddr, MPROF_16 | MPROF_READ);

  if (memaddr & 1)
    {
      except_handle (sim,EXCEPT_ALIGN, memaddr);
      return 0;
    }

  if (sim->config.debug.enabled)
    *breakpoint += check_debug_unit (sim,DebugLoadAddress, memaddr);	/* 28/05/01 CZ */

  phys_memaddr = dmmu_translate (sim,memaddr, 0);
  if (sim->except_pending)
    return 0;

  if (sim->config.dc.enabled)
    temp = dc_simulate_read (sim,phys_memaddr, memaddr, 2);
  else
    temp = evalsim_mem16 (sim,phys_memaddr, memaddr);

  if (sim->config.debug.enabled)
    *breakpoint += check_debug_unit (sim,DebugLoadData, temp);	/* MM170901 */

  return temp;
}

/* for simulator accesses, the ones that cpu wouldn't do
 *
 * STATISTICS OK.
 */
uint16_t
eval_direct16 (or1ksim *sim,oraddr_t memaddr, int through_mmu, int through_dc)
{
  oraddr_t phys_memaddr;
  struct dev_memarea *mem;

  if (memaddr & 1)
    {
      PRINTF ("%s:%d %s(): ERR unaligned access\n", __FILE__, __LINE__,
	      __FUNCTION__);
      return 0;
    }

  phys_memaddr = memaddr;

  if (through_mmu)
    phys_memaddr = peek_into_dtlb (sim, memaddr, 0, through_dc);

  if (through_dc)
    return dc_simulate_read (sim,phys_memaddr, memaddr, 2);
  else
    {
      if ((mem = verify_memoryarea (sim, phys_memaddr)))
	return mem->direct_ops.readfunc16 (sim, phys_memaddr & mem->size_mask,
					   mem->direct_ops.read_dat16);
      else
	PRINTF ("ERR: 16-bit read out of memory area: %" PRIxADDR
		" (physical: %" PRIxADDR "\n", memaddr, phys_memaddr);
    }

  return 0;
}

/* Returns 8-bit values from mem array.
 *
 * STATISTICS OK (only used for cpu_access, that is architectural access)
 */
uint8_t
eval_mem8 (or1ksim *sim,oraddr_t memaddr, int *breakpoint)
{
  uint8_t temp;
  oraddr_t phys_memaddr;

  if (sim->config.sim.mprofile)
    mprofile (sim,memaddr, MPROF_8 | MPROF_READ);

  if (sim->config.debug.enabled)
    *breakpoint += check_debug_unit (sim,DebugLoadAddress, memaddr);	/* 28/05/01 CZ */

  phys_memaddr = dmmu_translate (sim,memaddr, 0);
  if (sim->except_pending)
    return 0;

  if (sim->config.dc.enabled)
    temp = dc_simulate_read (sim,phys_memaddr, memaddr, 1);
  else
    temp = evalsim_mem8 (sim,phys_memaddr, memaddr);

  if (sim->config.debug.enabled)
    *breakpoint += check_debug_unit (sim,DebugLoadData, temp);	/* MM170901 */
  return temp;
}

/* for simulator accesses, the ones that cpu wouldn't do
 *
 * STATISTICS OK.
 */
uint8_t
eval_direct8 (or1ksim *sim,oraddr_t memaddr, int through_mmu, int through_dc)
{
  oraddr_t phys_memaddr;
  struct dev_memarea *mem;

  phys_memaddr = memaddr;

  if (through_mmu)
    phys_memaddr = peek_into_dtlb (sim, memaddr, 0, through_dc);

  if (through_dc)
    return dc_simulate_read (sim,phys_memaddr, memaddr, 1);
  else
    {
      if ((mem = verify_memoryarea (sim, phys_memaddr)))
	return mem->direct_ops.readfunc8 (sim, phys_memaddr & mem->size_mask,
					  mem->direct_ops.read_dat8);
      else
	PRINTF ("ERR: 8-bit read out of memory area: %" PRIxADDR
		" (physical: %" PRIxADDR "\n", memaddr, phys_memaddr);
    }

  return 0;
}

/* For cpu accesses
 *
 * NOTE: This function _is_ only called from set_mem32 below and
 * dc_simulate_write.  _Don't_ call it from anywere else.
 */
void
setsim_mem32 (or1ksim *sim,oraddr_t memaddr, oraddr_t vaddr, uint32_t value)
{
  struct dev_memarea *mem;

  if ((mem = verify_memoryarea (sim, memaddr)))
    {
      sim->cur_vadd = vaddr;
      sim->runtime.sim.mem_cycles += mem->ops.delayw;
      mem->ops.writefunc32 (sim, memaddr & mem->size_mask, value,
			    mem->ops.write_dat32);
#if DYNAMIC_EXECUTION
      dyn_checkwrite (memaddr);
#endif
    }
  else
    {
      PRINTF ("EXCEPTION: write out of memory (32-bit access to %" PRIxADDR
	      ")\n", memaddr);
      except_handle (sim,EXCEPT_BUSERR, vaddr);
    }
}

/* For cpu accesses
 *
 * NOTE: This function _is_ only called from set_mem16 below and
 * dc_simulate_write.  _Don't_ call it from anywere else.
 */
void
setsim_mem16 (or1ksim *sim,oraddr_t memaddr, oraddr_t vaddr, uint16_t value)
{
  struct dev_memarea *mem;

  if ((mem = verify_memoryarea (sim, memaddr)))
    {
      sim->cur_vadd = vaddr;
      sim->runtime.sim.mem_cycles += mem->ops.delayw;
      mem->ops.writefunc16 (sim, memaddr & mem->size_mask, value,
			    mem->ops.write_dat16);
#if DYNAMIC_EXECUTION
      dyn_checkwrite (memaddr);
#endif
    }
  else
    {
      PRINTF ("EXCEPTION: write out of memory (16-bit access to %" PRIxADDR
	      ")\n", memaddr);
      except_handle (sim,EXCEPT_BUSERR, vaddr);
    }
}

/* For cpu accesses
 *
 * NOTE: This function _is_ only called from set_mem8 below and
 * dc_simulate_write.  _Don't_ call it from anywere else.
 */
void
setsim_mem8 (or1ksim *sim,oraddr_t memaddr, oraddr_t vaddr, uint8_t value)
{
  struct dev_memarea *mem;

  if ((mem = verify_memoryarea (sim, memaddr)))
    {
      sim->cur_vadd = vaddr;
      sim->runtime.sim.mem_cycles += mem->ops.delayw;
      mem->ops.writefunc8 (sim, memaddr & mem->size_mask, value,
			   mem->ops.write_dat8);
#if DYNAMIC_EXECUTION
      dyn_checkwrite (memaddr);
#endif
    }
  else
    {
      PRINTF ("EXCEPTION: write out of memory (8-bit access to %" PRIxADDR
	      ")\n", memaddr);
      except_handle (sim,EXCEPT_BUSERR, vaddr);
    }
}

/* Set mem, 32-bit. Big endian version.
 *
 * STATISTICS OK. (the only suspicious usage is in sim-cmd.c,
 *                 where this instruction is used for patching memory,
 *                 wether this is cpu or architectual access is yet to
 *                 be decided)
 */
void
set_mem32 (or1ksim *sim,oraddr_t memaddr, uint32_t value, int *breakpoint)
{
  oraddr_t phys_memaddr;

  if (sim->config.sim.mprofile)
    mprofile (sim,memaddr, MPROF_32 | MPROF_WRITE);

  if (memaddr & 3)
    {
      except_handle (sim,EXCEPT_ALIGN, memaddr);
      return;
    }

  phys_memaddr = dmmu_translate (sim, memaddr, 1);;
  /* If we produced exception don't set anything */
  if (sim->except_pending)
    return;

  if (sim->config.debug.enabled)
    {
      *breakpoint += check_debug_unit (sim,DebugStoreAddress, memaddr);	/* 28/05/01 CZ */
      *breakpoint += check_debug_unit (sim,DebugStoreData, value);
    }

  if (sim->config.dc.enabled)
    dc_simulate_write (sim,phys_memaddr, memaddr, value, 4);
  else
    setsim_mem32 (sim,phys_memaddr, memaddr, value);

  if (sim->cur_area && sim->cur_area->log)
    fprintf (sim->cur_area->log, "[%" PRIxADDR "] -> write %08" PRIx32 "\n",
	     memaddr, value);
}

/*
 * STATISTICS NOT OK.
 */
void
set_direct32 (or1ksim *sim,oraddr_t memaddr, uint32_t value, int through_mmu,
	      int through_dc)
{
  oraddr_t phys_memaddr;
  struct dev_memarea *mem;

  if (memaddr & 3)
    {
      PRINTF ("%s:%d %s(): ERR unaligned access\n", __FILE__, __LINE__,
	      __FUNCTION__);
      return;
    }

  phys_memaddr = memaddr;

  if (through_mmu)
    {
      /* 0 - no write access, we do not want a DPF exception do we ;)
       */
      phys_memaddr = peek_into_dtlb (sim, memaddr, 1, through_dc);
    }

  if (through_dc)
    dc_simulate_write (sim,memaddr, memaddr, value, 4);
  else
    {
      if ((mem = verify_memoryarea (sim, phys_memaddr)))
	mem->direct_ops.writefunc32 (sim, phys_memaddr & mem->size_mask, value,
				     mem->direct_ops.write_dat32);
      else
	PRINTF ("ERR: 32-bit write out of memory area: %" PRIxADDR
		" (physical: %" PRIxADDR ")\n", memaddr, phys_memaddr);
    }

  if (sim->cur_area && sim->cur_area->log)
    fprintf (sim->cur_area->log, "[%" PRIxADDR "] -> DIRECT write %08" PRIx32 "\n",
	     memaddr, value);
}


/* Set mem, 16-bit. Big endian version. */

void
set_mem16 (or1ksim *sim,oraddr_t memaddr, uint16_t value, int *breakpoint)
{
  oraddr_t phys_memaddr;

  if (sim->config.sim.mprofile)
    mprofile (sim,memaddr, MPROF_16 | MPROF_WRITE);

  if (memaddr & 1)
    {
      except_handle (sim,EXCEPT_ALIGN, memaddr);
      return;
    }

  phys_memaddr = dmmu_translate (sim, memaddr, 1);;
  /* If we produced exception don't set anything */
  if (sim->except_pending)
    return;

  if (sim->config.debug.enabled)
    {
      *breakpoint += check_debug_unit (sim,DebugStoreAddress, memaddr);	/* 28/05/01 CZ */
      *breakpoint += check_debug_unit (sim,DebugStoreData, value);
    }

  if (sim->config.dc.enabled)
    dc_simulate_write (sim,phys_memaddr, memaddr, value, 2);
  else
    setsim_mem16 (sim,phys_memaddr, memaddr, value);

  if (sim->cur_area && sim->cur_area->log)
    fprintf (sim->cur_area->log, "[%" PRIxADDR "] -> write %04" PRIx16 "\n",
	     memaddr, value);
}

/*
 * STATISTICS NOT OK.
 */
void
set_direct16 (or1ksim *sim,oraddr_t memaddr, uint16_t value, int through_mmu,
	      int through_dc)
{
  oraddr_t phys_memaddr;
  struct dev_memarea *mem;

  if (memaddr & 1)
    {
      PRINTF ("%s:%d %s(): ERR unaligned access\n", __FILE__, __LINE__,
	      __FUNCTION__);
      return;
    }

  phys_memaddr = memaddr;

  if (through_mmu)
    {
      /* 0 - no write access, we do not want a DPF exception do we ;)
       */
      phys_memaddr = peek_into_dtlb (sim, memaddr, 0, through_dc);
    }

  if (through_dc)
    dc_simulate_write (sim,memaddr, memaddr, value, 2);
  else
    {
      if ((mem = verify_memoryarea (sim, phys_memaddr)))
	mem->direct_ops.writefunc16 (sim, phys_memaddr & mem->size_mask, value,
				     mem->direct_ops.write_dat16);
      else
	PRINTF ("ERR: 16-bit write out of memory area: %" PRIxADDR
		" (physical: %" PRIxADDR "\n", memaddr, phys_memaddr);
    }

  if (sim->cur_area && sim->cur_area->log)
    fprintf (sim->cur_area->log, "[%" PRIxADDR "] -> DIRECT write %04" PRIx16 "\n",
	     memaddr, value);
}

/* Set mem, 8-bit. */
void
set_mem8 (or1ksim *sim,oraddr_t memaddr, uint8_t value, int *breakpoint)
{
  oraddr_t phys_memaddr;

  if (sim->config.sim.mprofile)
    mprofile (sim,memaddr, MPROF_8 | MPROF_WRITE);

  phys_memaddr = memaddr;

  phys_memaddr = dmmu_translate (sim, memaddr, 1);;
  /* If we produced exception don't set anything */
  if (sim->except_pending)
    return;

  if (sim->config.debug.enabled)
    {
      *breakpoint += check_debug_unit (sim,DebugStoreAddress, memaddr);	/* 28/05/01 CZ */
      *breakpoint += check_debug_unit (sim,DebugStoreData, value);
    }

  if (sim->config.dc.enabled)
    dc_simulate_write (sim,phys_memaddr, memaddr, value, 1);
  else
    setsim_mem8 (sim,phys_memaddr, memaddr, value);

  if (sim->cur_area && sim->cur_area->log)
    fprintf (sim->cur_area->log, "[%" PRIxADDR "] -> write %02" PRIx8 "\n",
	     memaddr, value);
}

/*
 * STATISTICS NOT OK.
 */
void
set_direct8 (or1ksim *sim,oraddr_t memaddr, uint8_t value, int through_mmu, int through_dc)
{
  oraddr_t phys_memaddr;
  struct dev_memarea *mem;

  phys_memaddr = memaddr;

  if (through_mmu)
    {
      /* 0 - no write access, we do not want a DPF exception do we ;)
       */
      phys_memaddr = peek_into_dtlb (sim, memaddr, 0, through_dc);
    }

  if (through_dc)
    dc_simulate_write (sim,phys_memaddr, memaddr, value, 1);
  else
    {
      if ((mem = verify_memoryarea (sim, phys_memaddr)))
	mem->direct_ops.writefunc8 (sim, phys_memaddr & mem->size_mask, value,
				    mem->direct_ops.write_dat8);
      else
	PRINTF ("ERR: 8-bit write out of memory area: %" PRIxADDR
		" (physical: %" PRIxADDR "\n", memaddr, phys_memaddr);
    }

  if (sim->cur_area && sim->cur_area->log)
    fprintf (sim->cur_area->log, "[%" PRIxADDR "] -> DIRECT write %02" PRIx8 "\n",
	     memaddr, value);
}


/* set_program32 - same as set_direct32, but it also writes to memory that is
 *                 non-writeable to the rest of the sim.  Used to do program
 *                 loading.
 */
void
set_program32 (or1ksim *sim, oraddr_t memaddr, uint32_t value)
{
  struct dev_memarea *mem;

  if (memaddr & 3)
    {
      PRINTF ("%s(): ERR unaligned 32-bit program write\n", __FUNCTION__);
      return;
    }

  if ((mem = verify_memoryarea (sim, memaddr)))
    {
      mem->ops.writeprog32 (sim, memaddr & mem->size_mask, value,
			    mem->ops.writeprog32_dat);
    }
  else
    PRINTF ("ERR: 32-bit program load out of memory area: %" PRIxADDR "\n",
	    memaddr);
}

/* set_program8 - same as set_direct8, but it also writes to memory that is
 *                non-writeable to the rest of the sim.  Used to do program
 *                loading.
 */
void
set_program8 (or1ksim *sim, oraddr_t memaddr, uint8_t value)
{
  struct dev_memarea *mem;

  if ((mem = verify_memoryarea (sim, memaddr)))
    {
      mem->ops.writeprog8 (sim, memaddr & mem->size_mask, value,
			   mem->ops.writeprog8_dat);
    }
  else
    PRINTF ("ERR: 8-bit program load out of memory area: %" PRIxADDR "\n",
	    memaddr);
}


/*---------------------------------------------------------------------------*/
/*!Dump memory to the current output

   Output format is hex bytes, 16 bytes per line. Start each line with its
   address and (optionally) its symbolic name. Always end with a newline.

   There are all sorts of ways to trip this up, but they are unlikely. The
   validity of a memory area is taken from the address of the start of a line
   to be printed, so assumes the following 15 bytes are present. This could be
   fooled by ridiculous memory declarations.

   @param[in] from    Start address of the area of memory
   @param[in] to      End address of the area of memory                      */
/*---------------------------------------------------------------------------*/
void
dump_memory (or1ksim *sim,oraddr_t from, oraddr_t to)
{
  const int ROW_LEN = 16;
  oraddr_t i;			/* Row counter */

  for (i = from; i < to; i += ROW_LEN)
    {
      struct label_entry *entry = get_label (sim, i);
      oraddr_t j;		/* Index in row */

      PRINTF ("%" PRIxADDR, i);

      if (NULL != entry)
	{
	  int padding = 11 - strlen (entry->name);

	  PRINTF (" <%s>: ", entry->name);
	  PRINTF ("%*s ", padding < 0 ? 0 : padding, " ");
	}
      else
	{
	  PRINTF (":                ");
	}


      for (j = 0; j < ROW_LEN; j++)
	{
	  if (verify_memoryarea (sim, i + j))
	    {
	      PRINTF ("%02" PRIx8 " ", eval_direct8 (sim,i + j, 0, 0));
	    }
	  else
	    {
	      /* Not a valid memory area. Print Xs as required */
	      PRINTF ("XX ");
	    }
	}

      PRINTF ("\n");
    }
}				/* dump_memory() */


/*---------------------------------------------------------------------------*/
/*!Disassemble memory to the current output

   Output format is symbolic disassembly, one instruction per line. Start each
   line with its address and (optionally) its symbolic name.

   There are all sorts of ways to trip this up, but they are unlikely. The
   validity of a memory area is taken from the address of the start of a line
   to be printed, so assumes the following 3 bytes are present. This could be
   fooled by ridiculous memory declarations.

   @param[in] from    Start address of the area of memory
   @param[in] to      End address of the area of memory
   @param[in] nl      If non-zero (true) print a newline at the end of each
                      line                                                   */
/*---------------------------------------------------------------------------*/
void
disassemble_memory (or1ksim *sim,oraddr_t from, oraddr_t to, int nl)
{
  const int INSTR_LEN = 4;
  oraddr_t i;			/* Row counter */

  for (i = from; i < to; i += INSTR_LEN)
    {
      struct label_entry *entry = get_label (sim, i);

      PRINTF ("%" PRIxADDR, i);

      if (NULL != entry)
	{
	  int padding = 11 - strlen (entry->name);

	  PRINTF (" <%s>: ", entry->name);
	  PRINTF ("%*s ", padding < 0 ? 0 : padding, " ");
	}
      else
	{
	  PRINTF (":                ");
	}

      if (verify_memoryarea (sim, i))
	{
	  uint32_t insn = eval_direct32 (sim,i, 0, 0);
	  int index = insn_decode (sim, insn);

	  PRINTF ("%08" PRIx32 " ", insn);

	  if (index >= 0)
	    {
	      disassemble_insn (sim, insn);
	      PRINTF (" %s", sim->disassembled);
	    }
	  else
	    {
	      PRINTF ("<invalid>");
	    }

	}
      else
	{
	  /* Not a valid memory area. Print Xs as required */
	  PRINTF ("XXXXXXXX");
	}

      if (nl)
	{
	  PRINTF ("\n");
	}
    }
}				/* disassemble_memory() */


/* Closes files, etc. */

void
done_memory_table (or1ksim* sim)
{
  struct dev_memarea *ptmp;

  /* Check list of registered devices. */
  for (ptmp = sim->dev_list; ptmp; ptmp = ptmp->next)
    {
      if (ptmp->log)
	fclose (ptmp->log);
    }
}

/* Displays current memory configuration */

void
memory_table_status (or1ksim *sim)
{
  struct dev_memarea *ptmp;

  /* Check list of registered devices. */
  for (ptmp = sim->dev_list; ptmp; ptmp = ptmp->next)
    {
      PRINTF ("addr & %" PRIxADDR " == %" PRIxADDR " to %" PRIxADDR ", size %"
	      PRIx32 "\n", ptmp->addr_mask, ptmp->addr_compare,
	      ptmp->addr_compare | bit_mask (ptmp->size), ptmp->size);
      PRINTF ("\t");
      if (ptmp->ops.delayr >= 0)
	PRINTF ("read delay = %i cycles, ", ptmp->ops.delayr);
      else
	PRINTF ("reads not possible, ");

      if (ptmp->ops.delayw >= 0)
	PRINTF ("write delay = %i cycles", ptmp->ops.delayw);
      else
	PRINTF ("writes not possible");

      if (ptmp->log)
	PRINTF (", (logged)\n");
      else
	PRINTF ("\n");
    }
}

/* Outputs time in pretty form to dest string */

char *
generate_time_pretty (char *dest, long time_ps)
{
  int exp3 = 0;
  if (time_ps)
    {
      while ((time_ps % 1000) == 0)
	{
	  time_ps /= 1000;
	  exp3++;
	}
    }
  sprintf (dest, "%li%cs", time_ps, "pnum"[exp3]);
  return dest;
}
