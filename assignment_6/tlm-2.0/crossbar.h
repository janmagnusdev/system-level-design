#ifndef CROSSBAR_H_INCLUDED_
#define CROSSBAR_H_INCLUDED_

// include the submodules
#include "router.h"
#include "arbiter.h"

static
arbiter* arbiter_creator(const char* name, size_t){
  return new arbiter( name, 10, sc_core::SC_NS );
}

template< unsigned NumMasters, unsigned NumSlaves >
class crossbar
  : public sc_core::sc_module
{
public:

  typedef crossbar            this_type;
  typedef sc_core::sc_module  base_type;

  // sc_vector of sockets
  sc_core::sc_vector<tlm::tlm_initiator_socket<> > init_sockets;
  sc_core::sc_vector<tlm::tlm_target_socket<> >    target_sockets;

  SC_CTOR( crossbar )
    : init_sockets("init_sockets")
    , target_sockets("target_sockets")
    , routers("routers")
    , arbiters("arbiters")
  {
    init_sockets.init( NumSlaves);
    target_sockets.init( NumMasters );
    // create one router per master
    routers.init( NumMasters );
    // create one arbiter per slave
    arbiters.init( NumSlaves, arbiter_creator);

    // FILLME: instantiate the routers and bind them correctly to
    //         all arbiters
  }

private:

  sc_core::sc_vector< router > routers;
  sc_core::sc_vector< arbiter > arbiters;
};

#endif // CROSSBAR_H_INCLUDED_

