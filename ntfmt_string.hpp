#ifndef NTFMT_STRING_HPP_
#define NTFMT_STRING_HPP_

#include "ntfmt.hpp"

namespace ntfmt {
	template <typename charT>
	struct sink_string_fn_t: sink_fn_t<charT> {
		sink_string_fn_t(std::basic_string<charT> &str): str(str) { }
		int operator ()(charT const *s) {
			size_t const r = str.length();
			str += s;
			return str.length() - r;
		}
		int operator ()(charT c) {
			str += c;
			return static_cast<int>(c);
		}
	private:
		std::basic_string<charT> &str;
	};

	typedef sink_string_fn_t<char> sink_string;
	typedef sink_string_fn_t<wchar_t> sink_wstring;
}

#endif
