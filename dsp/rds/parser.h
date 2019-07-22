#ifndef INCLUDED_RDS_PARSER_H
#define INCLUDED_RDS_PARSER_H

#include "dsp/rds/api.h"
#include <gnuradio/block.h>

namespace gr {
namespace rds {

class RDS_API parser : virtual public gr::block
{
public:
	typedef boost::shared_ptr<parser> sptr;
	static sptr make(bool log, bool debug);

	virtual void reset() = 0;
};

} // namespace rds
} // namespace gr

#endif /* INCLUDED_RDS_PARSER_H */

