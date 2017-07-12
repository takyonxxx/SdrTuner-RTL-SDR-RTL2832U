
#ifndef UDP_SINK_F_H
#define UDP_SINK_F_H

#include <gnuradio/hier_block2.h>
#include <gnuradio/blocks/float_to_short.h>
#include <gnuradio/blocks/udp_sink.h>


class udp_sink_f;

typedef boost::shared_ptr<udp_sink_f> udp_sink_f_sptr;

udp_sink_f_sptr make_udp_sink_f(void);

class udp_sink_f : public gr::hier_block2
{
public:
    udp_sink_f(void);
    ~udp_sink_f();

    void start_streaming(const std::string host, int port);
    void stop_streaming(void);

private:
    gr::blocks::udp_sink::sptr        d_sink;  /*!< The gnuradio UDP sink. */
    gr::blocks::float_to_short::sptr  d_f2s;   /*!< Converts float to short. */

};


#endif // UDP_SINK_F_H
