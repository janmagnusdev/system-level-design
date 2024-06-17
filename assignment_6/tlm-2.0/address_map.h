
#ifndef ADDRESS_MAP_H_INCLUDED_
#define ADDRESS_MAP_H_INCLUDED_

#include <cstddef>
#include <vector>

struct address_map {
    typedef std::size_t    address_type;
    typedef std::ptrdiff_t size_type;
    typedef std::size_t    index_type;

    address_map() : entries() {}
    address_map( index_type slaves, const char* filename );

    index_type decode( address_type ) const;

    address_type get_start_address(index_type index) const;

    address_type get_local_address(index_type, address_type) const;
    address_type get_global_address(index_type, address_type) const;

    index_type operator()( address_type addr ) const
    { return decode( addr ); }

    index_type size() const
    { return entries.size(); }

    static const index_type npos
        = static_cast<index_type>(-1);

    void swap( address_map& that );

private:

    struct entry {
        address_type start;
        address_type end;
        size_type    size;
    };
    struct in_range;

    std::vector<entry> entries;
};

#endif // ADDRESS_MAP_H_INCLUDED_

/* vim: set ts=4 sw=4 tw=72 et :*/
