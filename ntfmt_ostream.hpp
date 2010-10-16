#ifndef NTFMT_OSTREAM_HPP_
#define NTFMT_OSTREAM_HPP_

#include "ntfmt.hpp"
#include <ostream>

namespace ntfmt {
	namespace detail {
		template <typename charT, typename Traits = std::char_traits<charT> >
		struct sink_ostream_fn_t: sink_fn_t {
			sink_ostream_fn_t(std::basic_ostream<charT, Traits> &os): os(os) { }
			void operator ()(char const *s) { os << s; }
			void operator ()(char c) { os << c; }
			void operator ()(wchar_t const *s) { os << s; }
			void operator ()(wchar_t c) { os << c; }
			std::basic_ostream<charT, Traits> &os;
		};
	}
	typedef sink_t< detail::sink_ostream_fn_t<char> > sink_ostream;
	typedef sink_t< detail::sink_ostream_fn_t<wchar_t> > sink_wostream;
}

#endif
