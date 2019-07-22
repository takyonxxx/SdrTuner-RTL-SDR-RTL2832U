#pragma once

#include <gnuradio/filter/fir_filter_ccf.h>
#include <gnuradio/hier_block2.h>

class fir_decim_cc;

typedef boost::shared_ptr<fir_decim_cc> fir_decim_cc_sptr;
fir_decim_cc_sptr make_fir_decim_cc(unsigned int decim);

class fir_decim_cc : public gr::hier_block2
{
    friend fir_decim_cc_sptr make_fir_decim_cc(unsigned int decim);

//protected:
public:
    fir_decim_cc(unsigned int decim);

public:
    ~fir_decim_cc();

private:
    gr::filter::fir_filter_ccf::sptr        fir1;
    gr::filter::fir_filter_ccf::sptr        fir2;
    gr::filter::fir_filter_ccf::sptr        fir3;
};
