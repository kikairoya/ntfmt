// this file describes how to print user-defined types

#include "ntfmt.hpp"
#include "ntfmt_stdio.hpp"
#include "ntfmt_float.hpp"

// now, we're going to print std::complex<double> with ntfmt.
#include <complex>

// define overloaded function-template 'ntfmt::ntfmt_printer' for custom formatter.
// ntfmt::ntfmt_printer takes 3 arguments.
// first argument is sink-function typed 'sink_fn_t<charT> &', and charT is template parameter takes 'char' or 'wchar_t'
// second argument is an object of or const-reference to user-defined type to print.
// third argument is an const-reference to flags_t tells formatting flags.
namespace ntfmt{
	template <typename charT, typename T>
	void ntfmt_printer(sink_fn_t<charT> &fn, std::complex<T> const &value, flags_t const &flags) {
		if (value.real()!=0) {
			fn << fmt(value.real(), flags);
			if (value.imag()>0) fn << NTFMT_CHR_PLUS;
		} else {
			if (value.imag()==0) fn << NTFMT_CHR_ZERO;
		}
		if (value.imag()!=0) {
			fn << fmt(value.imag(), flags);
			fn << NTFMT_CH_LIT('i');
		}
	}
}

int main() {
	ntfmt::sink_stdout() << std::complex<double>(347.35, -94.11) << '\n';
	ntfmt::sink_stdout() << std::complex<double>(-347.35, 94.11) << '\n';
	ntfmt::sink_stdout() << std::complex<double>(347.35, 0) << '\n';
	ntfmt::sink_stdout() << std::complex<double>(0, -94.11) << '\n';
	ntfmt::sink_stdout() << std::complex<double>(0, 0) << '\n';
}

