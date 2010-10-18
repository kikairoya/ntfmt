#ifndef NTFMT_STDIO_HPP_
#define NTFMT_STDIO_HPP_

#include "ntfmt.hpp"
#include <stdio.h>

namespace ntfmt {
	namespace detail {
		template <typename charT>
		struct sink_cfile_fn_t: sink_fn_t<charT> {
			sink_cfile_fn_t(FILE *const file): fp(file) { }
			int operator ()(char const *s) { return fputs(s, fp); }
			int operator ()(char c) { return fputc(c, fp); }
			int operator ()(wchar_t const *s) { return fputws(s, fp); }
			int operator ()(wchar_t c) { return fputwc(c, fp); }
		private:
			FILE *const fp;
		};
		template <typename charT>
		struct sink_stdout_fn_t: sink_cfile_fn_t<charT> {
			sink_stdout_fn_t(): sink_cfile_fn_t<charT>(stdout) { }
		};
		template <typename charT>
		struct sink_stderr_fn_t: sink_cfile_fn_t<charT> {
			sink_stderr_fn_t(): sink_cfile_fn_t<charT>(stderr) { }
		};
	}
	typedef sink_t< detail::sink_cfile_fn_t<char> > sink_cfile;
	typedef sink_t< detail::sink_stdout_fn_t<char> > sink_stdout;
	typedef sink_t< detail::sink_stderr_fn_t<char> > sink_stderr;
	typedef sink_t< detail::sink_cfile_fn_t<wchar_t> > sink_wcfile;
	typedef sink_t< detail::sink_stdout_fn_t<wchar_t> > sink_wstdout;
	typedef sink_t< detail::sink_stderr_fn_t<wchar_t> > sink_wstderr;
}

#endif
