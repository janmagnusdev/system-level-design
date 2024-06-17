#ifndef UTILS_H_INCLUDED_
#define UTILS_H_INCLUDED_

#include <systemc>
#include <tr1/functional>

class timer
{
public:
    timer()
        : updater( sc_core::sc_time_stamp )
        , start( updater() ) {
    }

    void reset() {
        start = updater();
    }

    sc_core::sc_time get() const {
        return updater() - start;
    }
private:
    std::tr1::function< sc_core::sc_time const & () > updater;
    sc_core::sc_time start;
};

#endif // UTILS_H_INCLUDED_

/* vim: set ts=4 sw=4 tw=72 et :*/
