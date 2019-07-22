#ifndef STEREO_DEMOD_H
#define STEREO_DEMOD_H

#include <gnuradio/hier_block2.h>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/filter/fir_filter_fcc.h>
#include <gnuradio/filter/fir_filter_fff.h>
#include <gnuradio/analog/pll_refout_cc.h>
#include <gnuradio/blocks/multiply_cc.h>
#include <gnuradio/blocks/multiply_ff.h>
#include <gnuradio/blocks/multiply_const_ff.h>
#include <gnuradio/blocks/complex_to_imag.h>
#include <gnuradio/blocks/add_ff.h>
#include <vector>
#include "dsp/lpf.h"
#include "dsp/resampler_xx.h"

 
class stereo_demod;

typedef boost::shared_ptr<stereo_demod> stereo_demod_sptr;


/*! \brief Return a shared_ptr to a new instance of stere_demod.
 *  \param quad_rate The input sample rate.
 *  \param audio_rate The audio rate.
 *  \param stereo On/off stereo mode.
 *
 * This is effectively the public constructor. To avoid accidental use
 * of raw pointers, stereo_demod's constructor is private.
 * make_stereo_demod is the public interface for creating new instances.
 */
stereo_demod_sptr make_stereo_demod(float quad_rate=120e3,
                                    float audio_rate=48e3,
                                    bool stereo=true, bool oirt=false);


/*! \brief FM stereo demodulator.
 *  \ingroup DSP
 *
 * This class implements the stereo demodulator for 87.5...108 MHz band.
 *
 */
class stereo_demod : public gr::hier_block2
{
    friend stereo_demod_sptr make_stereo_demod(float input_rate,
                                               float audio_rate,
                                               bool stereo,
                                               bool oirt);

protected:
    stereo_demod(float input_rate, float audio_rate, bool stereo, bool oirt);

public:
    ~stereo_demod();

private:
    /* GR blocks */
    gr::filter::fir_filter_fcc::sptr  tone;  /*!< Pilot tone BPF. */
    gr::analog::pll_refout_cc::sptr   pll;   /*!< Pilot tone PLL. */
    gr::blocks::multiply_cc::sptr subtone;   /*!< Stereo subtone. */
    gr::blocks::complex_to_imag::sptr lo;    /*!< Complex tone imag. */
    gr::filter::fir_filter_fff::sptr  lo2;   /*!< Subtone BPF. */
    gr::blocks::multiply_ff::sptr mixer;     /*!< Balance mixer. */
    lpf_ff_sptr lpf0;              /*!< Low-pass filter #0. */
    lpf_ff_sptr lpf1;              /*!< Low-pass filter #1. */
    resampler_ff_sptr audio_rr0;   /*!< Audio resampler #0. */
    resampler_ff_sptr audio_rr1;   /*!< Audio resampler #1. */
    gr::blocks::multiply_const_ff::sptr cdp; /*!< Channel delta (plus). */
    gr::blocks::multiply_const_ff::sptr cdm; /*!< Channel delta (minus). */
    gr::blocks::add_ff::sptr add0;           /*!< Left stereo channel. */
    gr::blocks::add_ff::sptr add1;           /*!< Right stereo channel. */

    /* other parameters */
    float d_input_rate;                  /*! Input rate. */
    float d_audio_rate;                  /*! Audio rate. */
    bool  d_stereo;                      /*! On/off stereo mode. */
    bool  d_oirt;
    std::vector<gr_complex> d_tone_taps; /*! Tone BPF taps. */
    std::vector<float> d_pll_taps;       /*! Subtone BPF taps. */
};


#endif // STEREO_DEMOD_H
