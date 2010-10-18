#ifndef NTFMT_FWD_HPP_
#define NTFMT_FWD_HPP_

#include "ntfmt_compat.hpp"
#include <wchar.h>
#include <boost/cstdint.hpp>

namespace ntfmt {
	struct flags_t {
		unsigned minus: 1;
		unsigned space: 1;
		unsigned alter: 1;
		unsigned zero: 1;
		unsigned plus: 1;
		unsigned capital: 1;
		unsigned character: 1;
		unsigned prec_enable: 1;
		unsigned precision: 8;
		unsigned radix: 5;
		unsigned width_enable: 1;
		unsigned exponential: 1;
		unsigned fixed: 1;
		unsigned width: 8;
	};

	template <typename T>
	struct fmt_t;
	struct sink_fn_t;

	namespace detail {
		inline flags_t const default_flags();
		template <typename charT>
		flags_t const decode_flags(charT const *fmtstr);
	}

	template <typename T>
	inline fmt_t<T> fmt(T const &v, char const *const f) { return fmt_t<T>(v, f); }
	template <typename T>
	inline fmt_t<T> fmt(T const &v, wchar_t const *const f) { return fmt_t<T>(v, f); }
	template <typename T>
	inline fmt_t<T> fmt(T const &v, flags_t const &f = detail::default_flags()) { return fmt_t<T>(v, f); }

	template <typename Fn>
	struct sink_t;

	namespace detail {
		struct noncopyable {
#ifdef BOOST_NO_DEFAULTED_FUNCTIONS
			noncopyable() { }
#else
			noncopyable() = default;
#endif
#ifdef BOOST_NO_DELETED_FUNCTIONS
		private:
			noncopyable(noncopyable const &);
			noncopyable &operator =(noncopyable const &);
#else
			noncopyable(noncopyable const &) = delete;
			noncopyable &operator =(noncopyable const &) = delete;
#endif
		};

		struct sink_fn_t: noncopyable {
			virtual int operator ()(char const *s) { for (int n = 0; *s && (*this)(*s++) >= 0; ++n) ; }
			virtual int operator ()(char c) = 0;
			virtual int operator ()(wchar_t const *s) { for (int n = 0; *s && (*this)(*s++); ++n) ; }
			virtual int operator ()(wchar_t c) { return (*this)(static_cast<char>(c)); }
			virtual ~sink_fn_t() { }
		};
	}

#define SPECIALIZE_NTFMT_FORMATTER(TYPE) \
	template <> \
	template <typename Fn> \
	void ::ntfmt::fmt_t<TYPE>::print(Fn &fn) const

}

#endif
