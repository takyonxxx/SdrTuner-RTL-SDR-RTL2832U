#ifndef INCLUDED_RDS_DECODER_H
#define INCLUDED_RDS_DECODER_H

#include "dsp/rds/api.h"
#include <gnuradio/sync_block.h>

namespace gr {
namespace rds {

class RDS_API decoder : virtual public gr::sync_block
{
public:
	typedef boost::shared_ptr<decoder> sptr;
	static sptr make(bool log, bool debug);
};

} // namespace rds
} // namespace gr

#endif /* INCLUDED_RDS_DECODER_H */
