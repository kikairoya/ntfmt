#ifndef NTFMT_STRING_HPP_
#define NTFMT_STRING_HPP_

#include "ntfmt.hpp"
#include <string>

namespace ntfmt {
	namespace detail {
		template <typename charT>
		struct sink_string_fn_t: sink_fn_t {
			sink_string_fn_t(std::basic_string<charT> &str): str(str) { }
			void operator ()(charT const *s) { str += s; }
			void operator ()(charT c) { str += c; }
			std::basic_string<charT> &str;
		};
	}
	typedef sink_t< detail::sink_string_fn_t<char> > sink_string;
	typedef sink_t< detail::sink_string_fn_t<wchar_t> > sink_wstring;
}

#endif
