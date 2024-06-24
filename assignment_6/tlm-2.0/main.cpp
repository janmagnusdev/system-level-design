
#include <systemc>

#include "master.h"
#include "ram.h"



int sc_main( int /* argc unused */, char* /* argv unused */[] )
{
    // TODO: initialisierung und verbindung von ports mit channels
    // passierte sonst in Testbench, das machen wir jetzt aber hier
    // master mit ram verbinden
    // init_socket.bind()
    sc_core::sc_start();

    return 0;
}

/* vim: set ts=4 sw=4 tw=72 et :*/
