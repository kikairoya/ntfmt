#include "../ntfmt.hpp"
#include "../ntfmt_string.hpp"
#include <stdio.h>

#define BOOST_TEST_MODULE integer_test
#include "test_main.hpp"

BOOST_AUTO_TEST_CASE(integer_basic) {
	TEST_FORMAT(0, "%d");
	TEST_FORMAT(100, "%d");
	TEST_FORMAT(100, "%+d");
	TEST_FORMAT(-100, "%d");
	TEST_FORMAT(-100, "%+d");
	TEST_FORMAT(0x123, "%x");
	TEST_FORMAT(01234567, "%o");
	TEST_FORMAT(0x7FFFFFFF, "%x");
	TEST_FORMAT(0x80000000, "%x");
	TEST_FORMAT(0xFFFFFFFF, "%x");
	TEST_FORMAT(static_cast<int>(0x7FFFFFFF), "%d");
	TEST_FORMAT(static_cast<int>(0x80000000), "%d");
	TEST_FORMAT(static_cast<int>(0xFFFFFFFF), "%d");
	TEST_FORMAT(0x7FFFFFFFFFFFFFFull, "%" fmtLL "x");
	TEST_FORMAT(0x800000000000000ull, "%" fmtLL "x");
	TEST_FORMAT(0xFFFFFFFFFFFFFFFull, "%" fmtLL "x");
	TEST_FORMAT(static_cast<long long>(0x7FFFFFFFFFFFFFFull), "%" fmtLL "d");
	TEST_FORMAT(static_cast<long long>(0x800000000000000ull), "%" fmtLL "d");
	TEST_FORMAT(static_cast<long long>(0xFFFFFFFFFFFFFFFull), "%" fmtLL "d");
	TEST_FORMAT(static_cast<unsigned>(-1), "%u");
	TEST_FORMAT(-1ll, "%" fmtLL "d");
	TEST_FORMAT(static_cast<unsigned long long>(-1ll), "%" fmtLL "u");
}

BOOST_AUTO_TEST_CASE(integer_width) {
	//                                     12345678901234567890
	TEST_FORMAT(0, "%1d");             // "0"
	TEST_FORMAT(0, "%2d");             // " 0"
	TEST_FORMAT(0, "%02d");            // "00"
	TEST_FORMAT(0, "%-2d");            // "0 "
	TEST_FORMAT(0, "% 2d");            // " 0"
	TEST_FORMAT(0, "% 3d");            // "  0"
	TEST_FORMAT(0, "%+2d");            // "+0"
	TEST_FORMAT(0, "%+3d");            // " +0"
	TEST_FORMAT(0x12345678, "%#9x");   // "0x12345678"
	TEST_FORMAT(0x12345678, "%#10x");  // "0x12345678"
	TEST_FORMAT(0x12345678, "%#11x");  // " 0x12345678"
	TEST_FORMAT(0x12345678, "%#09x");  // "0x12345678"
	TEST_FORMAT(0x12345678, "%#010x"); // "0x12345678"
	TEST_FORMAT(0x12345678, "%#011x"); // "0x012345678"
	TEST_FORMAT(0x12345678, "%#-10x"); // "0x12345678 "
	TEST_FORMAT(0x12345678, "%#-11x"); // "0x12345678 "
	TEST_FORMAT(0x12345678, "%#-12x"); // "0x12345678  "

	TEST_FORMAT( 12345678, "%7d");     // "12345678"
	TEST_FORMAT( 12345678, "%8d");     // "12345678"
	TEST_FORMAT(-12345678, "%8d");     // "-12345678"
	TEST_FORMAT( 12345678, "%9d");     // " 12345678"
	TEST_FORMAT(-12345678, "%9d");     // "-12345678"
	TEST_FORMAT(-12345678, "%10d");    // " -12345678"
	TEST_FORMAT( 12345678, "%07d");    // "12345678"
	TEST_FORMAT( 12345678, "%08d");    // "12345678"
	TEST_FORMAT(-12345678, "%08d");    // "-12345678"
	TEST_FORMAT( 12345678, "%09d");    // "012345678"
	TEST_FORMAT(-12345678, "%09d");    // "-12345678"
	TEST_FORMAT(-12345678, "%010d");   // "-012345678"
	TEST_FORMAT( 12345678, "%-7d");    // "12345678"
	TEST_FORMAT( 12345678, "%-8d");    // "12345678"
	TEST_FORMAT(-12345678, "%-8d");    // "-12345678"
	TEST_FORMAT( 12345678, "%-9d");    // "12345678 "
	TEST_FORMAT(-12345678, "%-9d");    // "-12345678"
	TEST_FORMAT(-12345678, "%-10d");   // "-12345678 "
	TEST_FORMAT( 12345678, "% 7d");    // " 12345678"
	TEST_FORMAT( 12345678, "% 8d");    // " 12345678"
	TEST_FORMAT( 12345678, "% 9d");    // " 12345678"
	TEST_FORMAT(-12345678, "% 9d");    // "-12345678"
	TEST_FORMAT( 12345678, "% 10d");   // "  12345678"
	TEST_FORMAT(-12345678, "% 10d");   // " -12345678"
	TEST_FORMAT(-12345678, "% 11d");   // "  -12345678"
	TEST_FORMAT( 12345678, "%+7d");    // "+12345678"
	TEST_FORMAT( 12345678, "%+8d");    // "+12345678"
	TEST_FORMAT( 12345678, "%+9d");    // "+12345678"
	TEST_FORMAT(-12345678, "%+9d");    // "-12345678"
	TEST_FORMAT( 12345678, "%+10d");   // " +12345678"
	TEST_FORMAT(-12345678, "%+10d");   // " -12345678"
	TEST_FORMAT(-12345678, "%+11d");   // "  -12345678"
	TEST_FORMAT( 12345678, "%0 7d");    // " 12345678"
	TEST_FORMAT( 12345678, "%0 8d");    // " 12345678"
	TEST_FORMAT( 12345678, "%0 9d");    // " 12345678"
	TEST_FORMAT(-12345678, "%0 9d");    // "-12345678"
	TEST_FORMAT( 12345678, "%0 10d");   // " 012345678"
	TEST_FORMAT(-12345678, "%0 10d");   // "-012345678"
	TEST_FORMAT(-12345678, "%0 11d");   // "-0012345678"
	TEST_FORMAT( 12345678, "%- 7d");    // " 12345678"
	TEST_FORMAT( 12345678, "%- 8d");    // " 12345678"
	TEST_FORMAT( 12345678, "%- 9d");    // " 12345678"
	TEST_FORMAT(-12345678, "%- 9d");    // "-12345678"
	TEST_FORMAT( 12345678, "%- 10d");   // " 12345678 "
	TEST_FORMAT(-12345678, "%- 10d");   // "-12345678 "
	TEST_FORMAT(-12345678, "%- 11d");   // "-12345678  "
	TEST_FORMAT( 12345678, "%0+7d");    // " 12345678"
	TEST_FORMAT( 12345678, "%0+8d");    // " 12345678"
	TEST_FORMAT( 12345678, "%0+9d");    // " 12345678"
	TEST_FORMAT(-12345678, "%0+9d");    // "-12345678"
	TEST_FORMAT( 12345678, "%0+10d");   // " 012345678"
	TEST_FORMAT(-12345678, "%0+10d");   // "-012345678"
	TEST_FORMAT(-12345678, "%0+11d");   // "-0012345678"
	TEST_FORMAT( 12345678, "%-+7d");    // "+12345678"
	TEST_FORMAT( 12345678, "%-+8d");    // "+12345678"
	TEST_FORMAT( 12345678, "%-+9d");    // "+12345678"
	TEST_FORMAT(-12345678, "%-+9d");    // "-12345678"
	TEST_FORMAT( 12345678, "%-+10d");   // "+12345678 "
	TEST_FORMAT(-12345678, "%-+10d");   // "-12345678 "
	TEST_FORMAT(-12345678, "%-+11d");   // "-12345678  "
}
BOOST_AUTO_TEST_CASE(integer_precision) {
	//                                      12345678901234567890
	TEST_FORMAT(0, "%.0d");             // ""
	TEST_FORMAT(0, "%.1d");             // "0"
	TEST_FORMAT(0, "%.2d");             // " 0"

	TEST_FORMAT(0x12345678, "%#.7x");   // "0x12345678"
	TEST_FORMAT(0x12345678, "%#.8x");   // "0x12345678"
	TEST_FORMAT(0x12345678, "%#.9x");   // "0x012345678"
	TEST_FORMAT(0x12345678, "%#-.7x");  // "0x12345678"
	TEST_FORMAT(0x12345678, "%#-.8x");  // "0x12345678"
	TEST_FORMAT(0x12345678, "%#-.9x");  // "0x012345678"

	TEST_FORMAT( 12345678, "%.7d");     // "12345678"
	TEST_FORMAT( 12345678, "%.8d");     // "12345678"
	TEST_FORMAT(-12345678, "%.8d");     // "-12345678"
	TEST_FORMAT( 12345678, "%.9d");     // "012345678"
	TEST_FORMAT(-12345678, "%.9d");     // "-012345678"
	TEST_FORMAT( 12345678, "%-.7d");    // "12345678"
	TEST_FORMAT( 12345678, "%-.8d");    // "12345678"
	TEST_FORMAT(-12345678, "%-.8d");    // "-12345678"
	TEST_FORMAT( 12345678, "%-.9d");    // "012345678"
	TEST_FORMAT(-12345678, "%-.9d");    // "-012345678"
	TEST_FORMAT( 12345678, "% .7d");    // " 12345678"
	TEST_FORMAT( 12345678, "% .8d");    // " 12345678"
	TEST_FORMAT( 12345678, "% .9d");    // " 012345678"
	TEST_FORMAT(-12345678, "% .9d");    // "-012345678"
	TEST_FORMAT( 12345678, "% .10d");   // " 0012345678"
	TEST_FORMAT(-12345678, "% .10d");   // "-0012345678"
	TEST_FORMAT( 12345678, "%+.7d");    // "+12345678"
	TEST_FORMAT( 12345678, "%+.8d");    // "+12345678"
	TEST_FORMAT( 12345678, "%+.9d");    // "+012345678"
	TEST_FORMAT(-12345678, "%+.9d");    // "-012345678"
	TEST_FORMAT( 12345678, "%+.10d");   // "+0012345678"
	TEST_FORMAT(-12345678, "%+.10d");   // "-0012345678"
	TEST_FORMAT( 12345678, "%- .7d");    // " 12345678"
	TEST_FORMAT( 12345678, "%- .8d");    // " 12345678"
	TEST_FORMAT( 12345678, "%- .9d");    // " 012345678"
	TEST_FORMAT(-12345678, "%- .9d");    // "-012345678"
	TEST_FORMAT( 12345678, "%- .10d");   // " 0012345678"
	TEST_FORMAT(-12345678, "%- .10d");   // "-0012345678"
	TEST_FORMAT( 12345678, "%-+.7d");    // "+12345678"
	TEST_FORMAT( 12345678, "%-+.8d");    // "+12345678"
	TEST_FORMAT( 12345678, "%-+.9d");    // "+012345678"
	TEST_FORMAT(-12345678, "%-+.9d");    // "-012345678"
	TEST_FORMAT( 12345678, "%-+.10d");   // "+0012345678"
	TEST_FORMAT(-12345678, "%-+.10d");   // "-0012345678"
}
BOOST_AUTO_TEST_CASE(integer_wid_prec) {
	//                                        12345678901234567890
	TEST_FORMAT(0, "%1.0d");              // " "
	TEST_FORMAT(0, "%1.1d");              // "0"
	TEST_FORMAT(0, "%2.1d");              // " 0"
	TEST_FORMAT(0, "%2.2d");              // "00"
	TEST_FORMAT(0, "%3.2d");              // " 00"

	TEST_FORMAT(0x12345678, "%#10.8x");   // "0x12345678"
	TEST_FORMAT(0x12345678, "%#10.9x");   // "0x012345678"
	TEST_FORMAT(0x12345678, "%#11.8x");   // " 0x12345678"
	TEST_FORMAT(0x12345678, "%#11.9x");   // "0x012345678"
	TEST_FORMAT(0x12345678, "%#12.9x");   // " 0x012345678"
	TEST_FORMAT(0x12345678, "%#-10.8x");  // "0x12345678"
	TEST_FORMAT(0x12345678, "%#-10.9x");  // "0x012345678"
	TEST_FORMAT(0x12345678, "%#-11.8x");  // "0x12345678 "
	TEST_FORMAT(0x12345678, "%#-11.9x");  // "0x012345678"
	TEST_FORMAT(0x12345678, "%#-12.8x");  // "0x12345678  "
	TEST_FORMAT(0x12345678, "%#-12.9x");  // "0x012345678 "

	TEST_FORMAT( 12345678, "%9.8d");      // " 12345678"
	TEST_FORMAT(-12345678, "%9.8d");      // "-12345678"
	TEST_FORMAT( 12345678, "%10.9d");     // " 012345678"
	TEST_FORMAT(-12345678, "%10.9d");     // "-012345678"
	TEST_FORMAT(-12345678, "%11.9d");     // " -012345678"
	TEST_FORMAT( 12345678, "%-9.8d");     // "12345678 "
	TEST_FORMAT(-12345678, "%-9.8d");     // "-12345678"
	TEST_FORMAT( 12345678, "%-10.9d");    // "012345678 "
	TEST_FORMAT(-12345678, "%-10.9d");    // "-012345678"
	TEST_FORMAT(-12345678, "%-11.9d");    // "-012345678 "
	TEST_FORMAT( 12345678, "% 9.8d");     // " 12345678"
	TEST_FORMAT( 12345678, "% 10.9d");    // " 012345678"
	TEST_FORMAT(-12345678, "% 10.9d");    // "-012345678"
	TEST_FORMAT(-12345678, "% 11.9d");    // " -012345678"
	TEST_FORMAT( 12345678, "%+9.8d");     // "+12345678"
	TEST_FORMAT( 12345678, "%+10.9d");    // "+012345678"
	TEST_FORMAT(-12345678, "%+10.9d");    // "-012345678"
	TEST_FORMAT(-12345678, "%+11.9d");    // " -012345678"
	TEST_FORMAT( 12345678, "%- 9.8d");    // " 12345678"
	TEST_FORMAT( 12345678, "%- 10.9d");   // " 012345678"
	TEST_FORMAT(-12345678, "%- 10.9d");   // "-012345678"
	TEST_FORMAT(-12345678, "%- 11.9d");   // "-012345678 "
	TEST_FORMAT( 12345678, "%-+9.8d");    // "+12345678"
	TEST_FORMAT( 12345678, "%-+10.9d");   // "+012345678"
	TEST_FORMAT(-12345678, "%-+10.9d");   // "-012345678"
	TEST_FORMAT(-12345678, "%-+11.9d");   // "-012345678 "
}
