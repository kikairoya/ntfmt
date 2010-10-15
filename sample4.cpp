// this file describes how to print user-defined types

#include "ntfmt.hpp"
#include "ntfmt_stdio.hpp"
#include "ntfmt_float.hpp"

// now, we're going to print std::complex<double> with ntfmt.
#include <complex>

// use SPECIALIZE_NTFMT_FORMATTER to define custom formatter.
// this macro defined in ntfmt_fwd.hpp
// passed 3 arguments implicitly, 'value', 'flags' and 'fn'.
//  value: type passed to macro argument
//  flags: const ntfmt::flags_t &
//  fn: sink_fn_t &
// to output, call fn(char) or fn(const char *) or each wchar_t version if sink supported.
// to redirect other formatter, call fmt('an object of any other type', 'flags in string or flags_t').print(fn).
SPECIALIZE_NTFMT_FORMATTER(std::complex<double>) {
	if (value.real()!=0) {
		fmt(value.real(), flags).print(fn);
		if (value.imag()>0) fn('+');
	} else {
		if (value.imag()==0) fn('0');
	}
	if (value.imag()!=0) {
		fmt(value.imag(), flags).print(fn);
		fn('i');
	}
}

int main() {
	ntfmt::sink_stdout() << std::complex<double>(347.35, -94.11) << '\n';
	ntfmt::sink_stdout() << std::complex<double>(-347.35, 94.11) << '\n';
	ntfmt::sink_stdout() << std::complex<double>(347.35, 0) << '\n';
	ntfmt::sink_stdout() << std::complex<double>(0, -94.11) << '\n';
	ntfmt::sink_stdout() << std::complex<double>(0, 0) << '\n';
}

