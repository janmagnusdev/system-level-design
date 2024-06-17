#ifndef SLAVE_H_INCLUDED_
#define SLAVE_H_INCLUDED_

#define SC_INCLUDE_DYNAMIC_PROCESSES

/* FILLME: add other required include files */

#include <vector>

struct ram
  : public sc_core::sc_module
  /* FILLME: protected inheritance from socket interface */
{
    typedef ram                this_type;
    typedef sc_core::sc_module base_type;

    ram( sc_core::sc_module_name, unsigned size );

    //* FILLME: socket definitions */

private:

    // helper functions
    bool is_invalid_address( unsigned addr ) const;
    void report_invalid_address( unsigned addr ) const;

    /* FILLME: implement socket interface functions */

    // member variables
    std::vector<unsigned> mem;

}; // ram

#endif // SLAVE_H_INCLUDED_

/* vim: set ts=4 sw=4 tw=72 et :*/
