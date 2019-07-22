#ifndef RX_DEMOD_AM_H
#define RX_DEMOD_AM_H

#include <gnuradio/hier_block2.h>
#include <gnuradio/blocks/complex_to_mag.h>
#include <gnuradio/filter/iir_filter_ffd.h>
#include <vector>


class rx_demod_am;

typedef boost::shared_ptr<rx_demod_am> rx_demod_am_sptr;

/*! \brief Return a shared_ptr to a new instance of rx_demod_am.
 *  \param quad_rate The input sample rate.
 *  \param dcr Enable DCR
 *
 * This is effectively the public constructor.
 */
rx_demod_am_sptr make_rx_demod_am(float quad_rate, bool dcr=true);

/*! \brief AM demodulator.
 *  \ingroup DSP
 *
 * This class implements the AM demodulator as envelope detector.
 * AM demodulation is simply a conversion from complex to magnitude.
 * This block implements an optional IIR DC-removal filter for the demodulated signal.
 *
 */
class rx_demod_am : public gr::hier_block2
{

public:
    rx_demod_am(float quad_rate, bool dcr=true); // FIXME: could be private
    ~rx_demod_am();

    void set_dcr(bool dcr);
    bool dcr();

private:
    /* GR blocks */
    gr::blocks::complex_to_mag::sptr    d_demod;  /*! AM demodulation (complex to magnitude). */
    gr::filter::iir_filter_ffd::sptr    d_dcr;    /*! DC removal (IIR high pass). */

    /* other parameters */
    bool   d_dcr_enabled;   /*! DC removal flag. */

    /* IIR DC-removal filter taps */
    std::vector<double> d_fftaps;   /*! Feed forward taps. */
    std::vector<double> d_fbtaps;   /*! Feed back taps. */

};


#endif // RX_DEMOD_AM_H
