
#include "ram.h"

#include <sstream> // std::stringstream

/* FILLME: constructor implementation */
// TODO: constructor is only defined in .h file, but we need to give it an implementation
// TODO: Bind us to our own export?

/* FILLME: b_transport implementation */
// TODO: Interessantester Teil der Aufgabe - wie macht man das?
// tlm_generic_payload& trans
// trans.get_address(), trans.get_data_length(), trans.get_streaming_width() !

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
