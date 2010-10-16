#ifndef NTFMT_STDIO_HPP_
#define NTFMT_STDIO_HPP_

#include "ntfmt.hpp"
#include <stdio.h>
#include <wchar.h>

namespace ntfmt {
	namespace detail {
		struct sink_cfile_fn_t: sink_fn_t {
			sink_cfile_fn_t(FILE *const file): fp(file) { }
			virtual ~sink_cfile_fn_t() { }
			void operator ()(char const *s) { fputs(s, fp); }
			void operator ()(char c) { fputc(c, fp); }
			void operator ()(wchar_t const *s) { fputws(s, fp); }
			void operator ()(wchar_t c) { fputwc(c, fp); }
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
