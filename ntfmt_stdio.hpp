#ifndef NTFMT_STDIO_HPP_
#define NTFMT_STDIO_HPP_

#include "ntfmt.hpp"
#include <stdio.h>

namespace ntfmt {
	namespace detail {
		struct sink_cfile_fn_t: sink_fn_t {
			sink_cfile_fn_t(FILE *const file): fp(file) { }
			virtual ~sink_cfile_fn_t() { }
			int operator ()(char const *s) { return fputs(s, fp); }
			int operator ()(char c) { return fputc(c, fp); }
			int operator ()(wchar_t const *s) { return fputws(s, fp); }
			int operator ()(wchar_t c) { return fputwc(c, fp); }
		private:
			FILE *const fp;
		};
		struct sink_stdout_fn_t: sink_cfile_fn_t {
			sink_stdout_fn_t(): sink_cfile_fn_t(stdout) { }
		};
		struct sink_stderr_fn_t: sink_cfile_fn_t {
			sink_stderr_fn_t(): sink_cfile_fn_t(stderr) { }
		};
	}
	typedef sink_t<detail::sink_cfile_fn_t> sink_cfile;
	typedef sink_t<detail::sink_stdout_fn_t> sink_stdout;
	typedef sink_t<detail::sink_stderr_fn_t> sink_stderr;
}

#endif
