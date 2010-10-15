// this file describes basic usage of ntfmt

#include "ntfmt.hpp"
#include "ntfmt_float.hpp"
#include "ntfmt_stdio.hpp"

int main() {
	using ntfmt::sink_stdout; // sink to stdout. sink_stderr is also usable.
	using ntfmt::fmt; // format marker

	// print integer
	sink_stdout() << -3839 << '\n'; // basic usage is similar to iostream...
	// print signed decimal
	sink_stdout() << fmt(6473, "%+8.5d") << '\n'; // but format directive is original.
	// print signed hexadecimal
	sink_stdout() << fmt(-0x12f, "%+#10.5x") << '\n';
	// print unsigned octal
	sink_stdout() << fmt<unsigned>(-06471, "%o") << '\n';
	// print float
	sink_stdout() << 285.6723 << '\n';
	// print float with scientific format
	sink_stdout() << fmt(853.842223, "%e") << '\n';
	// print float with fixed format
	sink_stdout() << fmt(0.74266684, "%.4f") << '\n';
	// print float with generic format
	sink_stdout() << fmt(-3572194.239, "%g") << '\n';
}
