#ifndef INCLUDED_RDS_PARSER_IMPL_H
#define INCLUDED_RDS_PARSER_IMPL_H

#include "dsp/rds/parser.h"
#include <gnuradio/thread/thread.h>

namespace gr {
namespace rds {

class parser_impl : public parser
{
public:
	parser_impl(bool log, bool debug);

private:
	~parser_impl();

	void reset();
	void send_message(long, std::string);
	void parse(pmt::pmt_t msg);
	double decode_af(unsigned int);
	void decode_optional_content(int, unsigned long int *);

	void decode_type0( unsigned int* group, bool B);
	void decode_type1( unsigned int* group, bool B);
	void decode_type2( unsigned int* group, bool B);
	void decode_type3( unsigned int* group, bool B);
	void decode_type4( unsigned int* group, bool B);
	void decode_type5( unsigned int* group, bool B);
	void decode_type6( unsigned int* group, bool B);
	void decode_type7( unsigned int* group, bool B);
	void decode_type8( unsigned int* group, bool B);
	void decode_type9( unsigned int* group, bool B);
	void decode_type10(unsigned int* group, bool B);
	void decode_type11(unsigned int* group, bool B);
	void decode_type12(unsigned int* group, bool B);
	void decode_type13(unsigned int* group, bool B);
	void decode_type14(unsigned int* group, bool B);
	void decode_type15(unsigned int* group, bool B);

	unsigned int   program_identification;
	unsigned char  program_type;
	unsigned char  pi_country_identification;
	unsigned char  pi_area_coverage;
	unsigned char  pi_program_reference_number;
	char           radiotext[65];
	char           program_service_name[9];
	bool           radiotext_AB_flag;
	bool           traffic_program;
	bool           traffic_announcement;
	bool           music_speech;
	bool           mono_stereo;
	bool           artificial_head;
	bool           compressed;
	bool           static_pty;
	bool           log;
	bool           debug;
	gr::thread::mutex d_mutex;
};

} /* namespace rds */
} /* namespace gr */

#endif /* INCLUDED_RDS_PARSER_IMPL_H */

