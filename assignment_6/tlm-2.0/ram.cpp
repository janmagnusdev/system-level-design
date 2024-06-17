
#include "ram.h"

#include <sstream> // std::stringstream

/* FILLME: constructor implementation */

/* FILLME: b_transport implementation */

/* FILLME: check for valid address request
 *         returns false, if address is valid,
 *         true (with error report) otherwise
 */
bool ram::is_invalid_address( unsigned addr ) const
{
}

/* helper function to report out-of-range error  */
void ram::report_invalid_address( unsigned addr ) const
{
    std::stringstream s;
    s << "Address "  << addr
      << " out of range [" << 0 << ","
      << mem.size() << ") "
      << "of RAM "<< name() << " - ignored";

    SC_REPORT_WARNING( "RAM/Out of range", s.str().c_str() );
}

/* vim: set ts=4 sw=4 tw=72 et :*/
