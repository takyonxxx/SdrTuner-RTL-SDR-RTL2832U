#include <gnuradio/hier_block2.h>
#include <gnuradio/blocks/float_to_short.h>
#include <gnuradio/blocks/udp_sink.h>
#include <gnuradio/io_signature.h>

#include "udp_sink_f.h"
/*
 * Create a new instance of gr::fcd::source_c_impl and return an
 * upcasted boost shared_ptr. This is effectively the public
 * constructor.
 */
udp_sink_f_sptr make_udp_sink_f()
{
    return gnuradio::get_initial_sptr(new udp_sink_f());
}

static const int MIN_IN = 1;  /*!< Mininum number of input streams. */
static const int MAX_IN = 1;  /*!< Maximum number of input streams. */
static const int MIN_OUT = 0; /*!< Minimum number of output streams. */
static const int MAX_OUT = 0; /*!< Maximum number of output streams. */

udp_sink_f::udp_sink_f()
    : gr::hier_block2("udp_sink_f",
                      gr::io_signature::make(MIN_IN, MAX_IN, sizeof(float)),
                      gr::io_signature::make(MIN_OUT, MAX_OUT, sizeof(float)))
{

    d_f2s = gr::blocks::float_to_short::make(1, 32767);
#ifdef GQRX_OS_MACX
    // There seems to be excessive packet loss (even to localhost) on OS X
    // unless the buffer size is limited.
    d_sink = gr::blocks::udp_sink::make(sizeof(short), "localhost", 7355, 512);
#else
    d_sink = gr::blocks::udp_sink::make(sizeof(short), "localhost", 7355);
#endif
    d_sink->disconnect();

    connect(self(), 0, d_f2s, 0);
    connect(d_f2s, 0, d_sink, 0);
}

udp_sink_f::~udp_sink_f()
{
}

/*! \brief Start streaming through the UDP sink
 *  \param host The hostname or IP address of the client.
 *  \param port The port used for the UDP stream
 */
void udp_sink_f::start_streaming(const std::string host, int port)
{
    d_sink->connect(host, port);
}


void udp_sink_f::stop_streaming(void)
{
    d_sink->disconnect();
}

