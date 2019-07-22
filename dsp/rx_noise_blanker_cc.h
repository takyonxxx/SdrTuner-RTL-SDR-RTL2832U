#ifndef RX_NB_CC_H
#define RX_NB_CC_H

#include <gnuradio/sync_block.h>
#include <gnuradio/gr_complex.h>
#include <boost/thread/mutex.hpp>

class rx_nb_cc;

typedef boost::shared_ptr<rx_nb_cc> rx_nb_cc_sptr;


/*! \brief Return a shared_ptr to a new instance of rx_nb_cc.
 *  \param sample_rate The samle rate (default = 96000).
 *  \param threshold Noise blanker threshold. Range 0.0 to 1.0 (TBC)
 *
 * This is effectively the public constructor for a new noise blanker block.
 * To avoid accidental use of raw pointers, the rx_nb_cc constructor is private.
 * make_rx_nb_cc is the public interface for creating new instances.
 */
rx_nb_cc_sptr make_rx_nb_cc(double sample_rate=96000.0, float thld1=3.3, float thld2=2.5);


/*! \brief Noise blanker block.
 *  \ingroup DSP
 *
 * This block implements noise blanking filters based on the noise blanker code
 * from DTTSP.

 *
 */
class rx_nb_cc : public gr::sync_block
{
    friend rx_nb_cc_sptr make_rx_nb_cc(double sample_rate, float thld1, float thld2);

protected:
    rx_nb_cc(double sample_rate, float thld1, float thld2);

public:
    ~rx_nb_cc();

    int work(int noutput_items,
             gr_vector_const_void_star &input_items,
             gr_vector_void_star &output_items);

    void set_sample_rate(double sample_rate) { d_sample_rate = sample_rate; }
    void set_nb1_on(bool nb1_on) { d_nb1_on = nb1_on; }
    void set_nb2_on(bool nb2_on) { d_nb2_on = nb2_on; }
    bool get_nb1_on() { return d_nb1_on; }
    bool get_nb2_on() { return d_nb2_on; }
    void set_threshold1(float threshold);
    void set_threshold2(float threshold);

private:
    void process_nb1(gr_complex *buf, int num);
    void process_nb2(gr_complex *buf, int num);

private:
    boost::mutex  d_mutex;  /*! Used to lock internal data while processing or setting parameters. */

    bool   d_nb1_on;        /*! Current NB1 status (true/false). */
    bool   d_nb2_on;        /*! Current NB2 status (true/false). */
    double d_sample_rate;   /*! Current sample rate. */
    float  d_thld_nb1;      /*! Current threshold for noise blanker 1 (1.0 to 20.0 TBC). */
    float  d_thld_nb2;      /*! Current threshold for noise blanker 2 (0.0 to 15.0 TBC). */
    float  d_avgmag_nb1;    /*! Average magnitude. */
    float  d_avgmag_nb2;    /*! Average magnitude. */
    gr_complex d_avgsig, d_delay[8];
    int    d_delidx, d_sigidx, d_hangtime;   // FIXME: need longer buffer for higher sampel rates?

};


#endif /* RX_NB_CC_H */
