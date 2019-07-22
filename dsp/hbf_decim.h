#ifndef INCLUDED_MYMOD_HBF_DECIM_H
#define INCLUDED_MYMOD_HBF_DECIM_H

#include <gnuradio/sync_decimator.h>
#include <gnuradio/types.h>
#include "filter/decimator.h"

class hbf_decim;
typedef boost::shared_ptr<hbf_decim> hbf_decim_sptr;
hbf_decim_sptr make_hbf_decim(unsigned int decim);

/**
 * Decimator block using half-band filters.
 */
class hbf_decim : virtual public gr::sync_decimator
{
    friend hbf_decim_sptr make_hbf_decim(unsigned int decim);

protected:
    hbf_decim(unsigned int decim);

public:
    ~hbf_decim();

    int work(int noutput_items,
             gr_vector_const_void_star &input_items,
             gr_vector_void_star &output_items);

private:
    Decimator          *dec;
    unsigned int        decimation;
};


#endif /* INCLUDED_MYMOD_HBF_DECIM_H */

