
#include <systemc>
#include <tlm.h>

#include "master.h"
#include "utils.h"

master::master( sc_core::sc_module_name /* unused */, 
                unsigned start_addr, unsigned end_addr )
: base_type()
, init_socket( "init_socket" )
, start( start_addr )
, end( end_addr )
{
    SC_THREAD( action );
    init_socket.bind( *this );
}

void master::action()
{
    // our data storage
    unsigned data;


    // prepare generic payload and delay
    tlm::tlm_generic_payload trans;
    trans.set_byte_enable_ptr( NULL );
    trans.set_byte_enable_length( 0 );
    trans.set_dmi_allowed( false );

    // set data options
    trans.set_data_length( sizeof(data) );
    trans.set_streaming_width( sizeof(data) );

    // set ptr to our data to payload
    trans.set_data_ptr( reinterpret_cast< unsigned char* >( &data ) );

    wait( 10, sc_core::SC_NS );

    // first, start write commands
    trans.set_command( tlm::TLM_WRITE_COMMAND );

    // transaction delay not used here
    sc_core::sc_time delay_unused = sc_core::SC_ZERO_TIME;

    for ( unsigned addr = start; addr <= end; addr++ ) {
        timer t;

        // send some random data
        data = rand();

        // update payload attributes for this transaction
        trans.set_address( addr );
        trans.set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );

        // access the connected target
        init_socket->b_transport( trans, delay_unused );

        wait( sc_core::SC_ZERO_TIME );

        std::cout << name()
            << " write addr=" << addr << ", data=" << data
            << " at " << sc_core::sc_time_stamp()
            << " (duration: " << t.get() << ")"
            << std::endl;
    }

    // update payload attributes for read access
    trans.set_command( tlm::TLM_READ_COMMAND );

    for ( unsigned addr = start; addr <= end; addr++ ) {
        timer t;

        // update payload attributes for this transaction
        trans.set_address( addr );
        trans.set_response_status( tlm::TLM_INCOMPLETE_RESPONSE );

        // access the connected target
        init_socket->b_transport( trans, delay_unused );

        std::cout << name()
            << " read addr=" << addr << ", data=" << data
            << " at " << sc_core::sc_time_stamp()
            << " (duration: " << t.get() << ")"
            << std::endl;

        wait( 1, sc_core::SC_NS );
    }
    // end of process
}

/* vim: set ts=4 sw=4 tw=72 et :*/
