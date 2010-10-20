#ifndef NTFMT_STDIO_HPP_
#define NTFMT_STDIO_HPP_

#include "ntfmt.hpp"
#include <stdio.h>

namespace ntfmt {
	namespace detail {
		inline int gfputs(char const *const s, FILE *const fp) { return fputs(s, fp); }
		inline int gfputc(char const c, FILE *const fp) { return fputc(c, fp); }
		inline int gfputs(wchar_t const *const s, FILE *const fp) { return fputws(s, fp); }
		inline int gfputc(wchar_t const c, FILE *const fp) { return fputwc(c, fp); }
	}
	template <typename charT>
	struct sink_cfile_fn_t: sink_fn_t<charT> {
		sink_cfile_fn_t(FILE *const file): fp(file) { }
		int operator ()(charT const *s) { return detail::gfputs(s, fp); }
		int operator ()(charT c) { return detail::gfputc(c, fp); }
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
	typedef sink_t< sink_cfile_fn_t<char> > sink_cfile;
	typedef sink_t< sink_stdout_fn_t<char> > sink_stdout;
	typedef sink_t< sink_stderr_fn_t<char> > sink_stderr;
	typedef sink_t< sink_cfile_fn_t<wchar_t> > sink_wcfile;
	typedef sink_t< sink_stdout_fn_t<wchar_t> > sink_wstdout;
	typedef sink_t< sink_stderr_fn_t<wchar_t> > sink_wstderr;
}

#endif
