#define BOOST_TEST_NO_LIB

#ifdef BOOST_TEST_NO_LIB
#include <boost/test/included/unit_test.hpp>
#else
#include <boost/test/unit_test.hpp>
#endif

#if defined(_WIN32) && !defined(__CYGWIN__)
#define fmtLL "I64"
#else
#define fmtLL "ll"
#endif
#define TEST_FORMAT(val, fmtstr) \
	do { \
		std::string s; \
		char buf[1030]; \
		ntfmt::sink_string(s) << "'" << ntfmt::fmt(val, fmtstr) << "'"; \
		sprintf(buf, "'" fmtstr "'", val); \
		BOOST_CHECK_EQUAL(((void)val, (void)fmtstr, s), buf); \
	} while (0)

