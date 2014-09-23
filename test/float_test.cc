#include "../ntfmt.hpp"
#include "../ntfmt_string.hpp"
#include <stdio.h>

#define BOOST_TEST_MODULE float_test
#include "test_main.hpp"

BOOST_AUTO_TEST_CASE(float_f_basic) {
	TEST_FORMAT( 0.0, "%f");
	//TEST_FORMAT(-0.0, "%f"); // result is difference between msvcrt and newlib
	TEST_FORMAT( 0.5, "%f");
	TEST_FORMAT(-0.5, "%f");
	TEST_FORMAT( 1.0, "%f");
	TEST_FORMAT(-1.0, "%f");
	TEST_FORMAT(0.12345678, "%f");
	TEST_FORMAT(1.12345678, "%f");
	TEST_FORMAT(12345678.12345678, "%f");
	{ // TEST_FORMAT(std::numeric_limits<double>::max(), "%f");
		// print of very-large magnitude values are different on runtime-library implementation
		const double val = std::numeric_limits<double>::max();
		std::string s;
		char buf[1030];
		ntfmt::sink_string(s) << "'" << ntfmt::fmt(val, "%f") << "'";
		sprintf(buf, "'%f'", val);
		BOOST_CHECK_EQUAL(((void)val, (void)"%f", s.substr(0, 12)), std::string(buf).substr(0, 12));
	}
	TEST_FORMAT(std::numeric_limits<double>::min(), "%f");
	//TEST_FORMAT(std::numeric_limits<double>::infinity(), "%f");
	//TEST_FORMAT(std::numeric_limits<double>::quiet_NaN(), "%f");
	//TEST_FORMAT(std::numeric_limits<double>::signaling_NaN(), "%f");
}

BOOST_AUTO_TEST_CASE(float_f_width) {
	TEST_FORMAT(0.0, "%8f"); // "0.000000"
	TEST_FORMAT(0.0, "%9f"); // " 0.000000"
	TEST_FORMAT(0.5, "%8f"); // "0.500000"
	TEST_FORMAT(0.5, "%9f"); // " 0.500000"
	TEST_FORMAT(1.0, "%8f"); // "1.000000"
	TEST_FORMAT(1.0, "%9f"); // " 1.000000"
}

BOOST_AUTO_TEST_CASE(float_f_precision) {
	TEST_FORMAT(0.0, "%.0f");  // "0"
	TEST_FORMAT(0.0, "%.1f");  // " 0.0"
	TEST_FORMAT(0.05, "%.0f");  // "0"
//	TEST_FORMAT(0.05, "%.1f");  // "0.0" // printf returns "0.1" due to binarize error but shoud be 0.0 by NR
	TEST_FORMAT(0.05, "%.2f");  // "0.05"
	TEST_FORMAT(0.5, "%.0f");  // "0"
	TEST_FORMAT(0.5, "%.1f");  // " 0.5"
	TEST_FORMAT(0.5, "%.2f");  // " 0.50"
	TEST_FORMAT(0.55, "%.1f"); // "0.6"
	TEST_FORMAT(0.55, "%.2f"); // " 0.55"
	TEST_FORMAT(0.55, "%.3f"); // " 0.550"
	TEST_FORMAT(1.0, "%.0f");  // "1"
	TEST_FORMAT(1.0, "%.1f");  // " 1.0"
	TEST_FORMAT(1.0, "%.2f");  // "1.00"
	TEST_FORMAT(1.55, "%.0f"); // " 2"
	TEST_FORMAT(1.55, "%.1f"); // " 1.6"
	TEST_FORMAT(1.55, "%.2f"); // "1.55"
	TEST_FORMAT(1.55, "%.3f"); // " 1.550"
}
