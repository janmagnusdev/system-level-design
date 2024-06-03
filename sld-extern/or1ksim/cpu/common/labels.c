/* abstract.c -- Abstract entities, handling labels

   Copyright (C) 2001 Marko Mlinar, markom@opencores.org
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

/* Abstract memory and routines that go with this. I need to add all sorts of
   other abstract entities. Currently we have only memory. */


/* Autoconf and/or portability configuration */
#include "config.h"
#include "port.h"

/* System includes */
#include <stdlib.h>

/* Package includes */
#include "labels.h"

void
init_labels (or1ksim *sim)
{
  int i;
  for (i = 0; i < LABELS_HASH_SIZE; i++)
    sim->label_hash[i] = NULL;
}

void
add_label (or1ksim* sim, oraddr_t addr, char *name)
{
  struct label_entry **tmp;
  tmp = &(sim->label_hash[addr % LABELS_HASH_SIZE]);
  for (; *tmp; tmp = &((*tmp)->next));
  *tmp = malloc (sizeof (**tmp));
  (*tmp)->name = malloc (strlen (name) + 1);
  (*tmp)->addr = addr;
  strcpy ((*tmp)->name, name);
  (*tmp)->next = NULL;
}

struct label_entry *
get_label (or1ksim* sim, oraddr_t addr)
{
  struct label_entry *tmp = sim->label_hash[addr % LABELS_HASH_SIZE];
  while (tmp)
    {
      if (tmp->addr == addr)
	return tmp;
      tmp = tmp->next;
    }
  return NULL;
}

struct label_entry *
find_label (or1ksim* sim, char *name)
{
  int i;
  for (i = 0; i < LABELS_HASH_SIZE; i++)
    {
      struct label_entry *tmp = sim->label_hash[i % LABELS_HASH_SIZE];
      while (tmp)
	{
	  if (strcmp (tmp->name, name) == 0)
	    return tmp;
	  tmp = tmp->next;
	}
    }
  return NULL;
}

/* Searches mem array for a particular label and returns label's address.
   If label does not exist, returns 0. */
oraddr_t
eval_label (or1ksim* sim, char *name)
{
  struct label_entry *le;
  char *plus;
  char *minus;
  int positive_offset = 0;
  int negative_offset = 0;

  if ((plus = strchr (name, '+')))
    {
      *plus = '\0';
      positive_offset = atoi (++plus);
    }

  if ((minus = strchr (name, '-')))
    {
      *minus = '\0';
      negative_offset = atoi (++minus);
    }
  le = find_label (sim, name);
  if (!le)
    return 0;

  return le->addr + positive_offset - negative_offset;
}

void
init_breakpoints (or1ksim* sim)
{
  sim->breakpoints = 0;
}

void
add_breakpoint (or1ksim* sim, oraddr_t addr)
{
  struct breakpoint_entry *tmp;
  tmp = (struct breakpoint_entry *) malloc (sizeof (struct breakpoint_entry));
  tmp->next = sim->breakpoints;
  tmp->addr = addr;
  sim->breakpoints = tmp;
}

void
remove_breakpoint (or1ksim* sim, oraddr_t addr)
{
  struct breakpoint_entry **tmp = &sim->breakpoints;
  while (*tmp)
    {
      if ((*tmp)->addr == addr)
	{
	  struct breakpoint_entry *t = *tmp;
	  (*tmp) = t->next;
	  free (t);
	}
      else
	tmp = &((*tmp)->next);
    }
}

void
print_breakpoints (or1ksim* sim)
{
  struct breakpoint_entry **tmp = &sim->breakpoints;
  int i = 1;
  printf ("---[breakpoints]------------------\n");
  while (*tmp)
    {
      printf ("Breakpoint %i at 0x%" PRIxADDR "\n", i, (*tmp)->addr);
      tmp = &((*tmp)->next);
    }
  printf ("---[breakpoints end]--------------\n");
}

int
has_breakpoint (or1ksim* sim, oraddr_t addr)
{
  struct breakpoint_entry *tmp = sim->breakpoints;
  while (tmp)
    {
      if (tmp->addr == addr)
	return 1;
      tmp = tmp->next;
    }
  return 0;
}
