#ifndef BUS_H_INCLUDED_
#define BUS_H_INCLUDED_

#include "address_map.h"

#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <systemc>

#include <tlm_utils/multi_passthrough_target_socket.h>
#include <tlm_utils/multi_passthrough_initiator_socket.h>
#include <tlm.h>

// Interconnect component
struct bus
: public sc_core::sc_module
{
    typedef bus                this_type;
    typedef sc_core::sc_module base_type;

    tlm_utils::multi_passthrough_initiator_socket<this_type> init_socket;
    tlm_utils::multi_passthrough_target_socket<this_type>    target_socket;

    bus( sc_core::sc_module_name = sc_core::sc_gen_unique_name("bus") );
    ~bus();

private:
    // Loosely-Timed (Blocking Transport)
    virtual void b_transport( int id, tlm::tlm_generic_payload& trans,
                              sc_core::sc_time& delay );

    // stuff for address decoding
    virtual void end_of_elaboration();

    address_map targets;
};

#endif // BUS_H_INCLUDED_

/* vim: set ts=4 sw=4 tw=72 et :*/
