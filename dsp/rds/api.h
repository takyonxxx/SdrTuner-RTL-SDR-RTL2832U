#ifndef INCLUDED_RDS_API_H
#define INCLUDED_RDS_API_H

#include <gnuradio/attributes.h>

#ifdef gnuradio_RDS_EXPORTS
#  define RDS_API __GR_ATTR_EXPORT
#else
#  define RDS_API __GR_ATTR_IMPORT
#endif

#endif /* INCLUDED_RDS_API_H */
