#include "data_types.h"
#include <sstream>

void sc_trace( sc_core::sc_trace_file* f, sensor_data const & sd,
               std::string const & s )
{
    sc_core::sc_trace( f, sd.movement, s + ".movement" );
    for (unsigned i = 1; i < NUMBER_OF_SENSORS; i++) {
        std::stringstream name;
        name << s << ".sensors[" << i << "]";
        sc_core::sc_trace( f, sd.sensor[i], name.str() );
    }
}

void sc_trace( sc_core::sc_trace_file* f, control_data const & cd,
               std::string const & s )
{
    sc_core::sc_trace( f, cd.movement, s + ".movement" );
    sc_core::sc_trace( f, cd.rotation, s + ".rotation" );
}

std::ostream& operator<<(std::ostream& os, const sensor_data& sd)
{
    os << "(movement=" << sd.movement << ",sensors=[" << sd.sensor[0];
    if (NUMBER_OF_SENSORS > 1)
        for (unsigned i = 1; i < NUMBER_OF_SENSORS; i++)
            os << "," << sd.sensor[i];
    os << "])";
    return os;
}

std::ostream& operator<<( std::ostream& os, const control_data& cd )
{
    os << "(movement=" << cd.movement << ",rotation=" << cd.rotation << ")";
    return os;
}

bool operator==( const control_data& left, const control_data& right )
{
    return ( left.movement == right.movement ) &&
           ( left.rotation == right.rotation );
}

bool operator==( const sensor_data& left, const sensor_data& right )
{
    if ( left.movement != right.movement )
        return false;
    else for ( unsigned i = 0; i < NUMBER_OF_SENSORS; i++ )
        if ( left.sensor[i] != right.sensor[i] )
            return false;
    return true;
}
