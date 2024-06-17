
#include <systemc>
#include <tlm.h>

#include "bus.h"


bus::bus( sc_core::sc_module_name /* unused */ )
: base_type()
, init_socket("init_socket")
, target_socket("target_socket")
{
    target_socket.register_b_transport(this, &this_type::b_transport);
}

bus::~bus()
{ }

// FILLME: implement bus::b_transport
//   NOTE: You can use 'targets.decode()' from the address_map class for
//         the actual decoding.  'address_map::npos' is returned, if the
//         requested address is not mapped.



// stuff for address decoding
void bus::end_of_elaboration()
{
    // reset address map with correct data
    address_map( init_socket.size(), "mem_map.txt" ).swap( targets );

    sc_assert( init_socket.size() == targets.size() );
}

/* vim: set ts=4 sw=4 tw=72 et :*/
