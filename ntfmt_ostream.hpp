#ifndef NTFMT_OSTREAM_HPP_
#define NTFMT_OSTREAM_HPP_

#include "ntfmt.hpp"
#include <ostream>

namespace ntfmt {
	template <typename charT, typename Traits = std::char_traits<charT> >
	struct sink_ostream_fn_t: sink_fn_t<charT> {
		sink_ostream_fn_t(std::basic_ostream<charT, Traits> &os): os(os) { }
		int operator ()(char const *s) { os << s; return strlen(s); }
		int operator ()(char c) { os << c; return c; }
		int operator ()(wchar_t const *s) { os << s; return wcslen(s); }
		int operator ()(wchar_t c) { os << c; return c;}
	private:
		std::basic_ostream<charT, Traits> &os;
	};
	typedef sink_ostream_fn_t<char> sink_ostream;
	typedef sink_ostream_fn_t<wchar_t> sink_wostream;
}

#endif
