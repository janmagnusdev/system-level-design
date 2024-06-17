#ifndef MASTER_H_INCLUDED_
#define MASTER_H_INCLUDED_

#include <systemc>
#include <tlm.h>

struct master
: public sc_core::sc_module
, protected tlm::tlm_bw_transport_if<> 
{
    typedef master             this_type;
    typedef sc_core::sc_module base_type;

    SC_HAS_PROCESS(this_type);
    master( sc_core::sc_module_name,
            unsigned start_addr, unsigned end_addr );

    // process implementation
    void action();

    tlm::tlm_initiator_socket<> init_socket;

private: // implementation details

    // tlm_bw_transport_if methods (neccessary for non-blocking or
    // debug interfaces, but not used here)
    virtual tlm::tlm_sync_enum
    nb_transport_bw( tlm::tlm_generic_payload&, tlm::tlm_phase&,
                     sc_core::sc_time& )
    { return tlm::TLM_COMPLETED; }

    virtual void invalidate_direct_mem_ptr( sc_dt::uint64,
                                            sc_dt::uint64 )
    { }

    // member variables
    unsigned start;
    unsigned end;
}; // master

#endif // MASTER_H_INCLUDED_

/* vim: set ts=4 sw=4 tw=72 et :*/
