// this file describes basic usage with C++0x mode

#include "ntfmt.hpp"
#include "ntfmt_stdio.hpp"

int main() {
	using ntfmt::sink_stdout;
	using ntfmt::fmt;

	// pass all arguments to format().
	sink_stdout().format(-3839, '\n', fmt(6473, "%+8.5d"), '\n', fmt(-0x12f, "%+#10.5x"), '\n');
}
