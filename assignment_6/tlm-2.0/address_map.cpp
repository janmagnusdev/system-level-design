
#include "address_map.h"

#include <fstream>       // std::fstream
#include <sstream>       // std::stringstream
#include <string>        // std::string
#include <algorithm>     // std::find_if, std::ptr_fun
#include <iostream>      // std::cout, std::endl


address_map::address_map( index_type slaves, const char* filename )
    : entries( slaves )
{
    std::fstream mem_map( filename );
    std::string line;

    index_type i = 0, slave_start = 0, slave_end = 0;

    while ( std::getline(mem_map, line) ) {
        // trim whitespace from the beginning of the line
        line.erase(line.begin(),
                   std::find_if(line.begin(), line.end(),
                                std::not1( std::ptr_fun< int, int >(std::isspace ))
                               )
                  );

        // skip empty lines
        if (line.empty()) continue;

        // skip comment lines
        if (line[0] == '#') continue;

        std::stringstream(line) >> std::dec >> i
                                >> std::hex >> slave_start >> slave_end;
        if( i >= slaves ) {
            std::cerr <<  "Bus ERROR: slave number " << std::dec << i << " does not exist";
        } else {
            entries[i].start = slave_start;
            entries[i].end   = slave_end;

            entries[i].size  = 1U + ( slave_end - slave_start );

            std::cout << "Bus slave " << std::dec << i
                      << " starts 0x"   << std::hex << entries[i].start
                      << " ends 0x"     << std::hex << entries[i].end
                      << " size 0x"     << std::hex << entries[i].size
                      << std::endl;
            std::cout << std::dec;
        }
    }
}

void address_map::swap( address_map & that )
{
    that.entries.swap( entries );
}


struct address_map::in_range
{
    explicit in_range( address_type addr )
    : addr( addr )
    {}

    bool operator()( const entry& e ) const
    { return ( e.start <= addr ) && ( addr <= e.end ) ; }

private:
    address_type addr;
};

address_map::index_type address_map::decode( address_type addr ) const
{
    std::vector<entry>::const_iterator index
        = std::find_if( entries.begin(), entries.end(), in_range(addr) );

    if( index != entries.end() )
        return index - entries.begin();

    return npos; // not found
}

address_map::address_type
address_map::get_start_address(address_map::index_type index) const
{
    if(index >= this->size()) {
        return npos; // not found
    }
    return entries[index].start;
}

address_map::address_type
address_map::get_local_address(index_type index, address_type address) const
{
    return address - get_start_address(index);
}

address_map::address_type
address_map::get_global_address(index_type index, address_type address) const
{
    return address + get_start_address(index);
}

/* vim: set ts=4 sw=4 tw=72 et :*/
