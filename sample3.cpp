// this file describes basic usage of sink_strbuf

#include "ntfmt.hpp"
#include "ntfmt_stdio.hpp"

int main() {
	using ntfmt::sink_strbuf;
	using ntfmt::fmt;

	{
		char buf[10]; // output buffer as char-array
		sink_strbuf(&buf) << 3247.554; // pass "pointer to array" to sink_strbuf, and other usage is same to sink_stdout
		// zero-terminated automatically.
		puts(buf);
	} {
		char buf[4]; // small buffer
		sink_strbuf(&buf) << 327489231; // too long output...
		puts(buf); // is truncated safely.
	} {
		unsigned const N = 4;
		char *buf = new char[N]; // dynamic allocation
		sink_strbuf(buf, N) << -382143; // pass "first address of buffer" and "buffer size in bytes".
		puts(buf); // of course, truncated automatically.
		delete[] buf;
	}
	// I will add 'sink to std::vector' or 'sink to std::string', 'sink to std::iostream'.
}
