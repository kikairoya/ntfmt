#ifndef NTFMT_HPP_
#define NTFMT_HPP_

#include "ntfmt_fwd.hpp"

#include <string.h>
#include <ctype.h>
#include <cstdlib>
#include <limits>
#include <utility>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/type_traits/is_unsigned.hpp>
#include <boost/type_traits/is_signed.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/type_traits/make_unsigned.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/logical.hpp>

#include <boost/preprocessor.hpp>

namespace ntfmt {
	namespace detail {
#ifndef BOOST_DINKUMWARE_STDLIB
		using std::abs;
#endif
		using std::numeric_limits;
		using boost::mpl::bool_;
		using boost::mpl::and_;
		using boost::mpl::or_;
		using boost::mpl::not_;
		using boost::mpl::if_c;
		using boost::enable_if;
		using boost::is_convertible;
		using boost::is_unsigned;
		using boost::is_signed;
		using boost::is_floating_point;
		using boost::is_pointer;
		using boost::make_unsigned;

		struct sink_strbuf_fn_t: sink_fn_t {
			template <size_t N>
			sink_strbuf_fn_t(char (*const &buf)[N]): buf(*buf), p(*buf), size(N) { }
			sink_strbuf_fn_t(char *const buf, size_t const size): buf(buf), p(buf), size(size) { }
			void operator ()(char const *s) { while (*s && p < buf+size-1) *p++ = *s++, *p = 0; }
			void operator ()(char c) { if (p < buf+size-1) *p++ = c, *p = 0; }
			char *const buf;
			char *p;
			size_t const size;
		};

		inline flags_t const default_flags() {
			flags_t f[1] = { };
			f[0].radix = 10;
			return f[0];
		}

		template <typename charT>
		flags_t const decode_flags(charT const *fmtstr) {
			flags_t f = default_flags();
			if (!fmtstr || *fmtstr++!='%') return f;
		cont:
			if (!*fmtstr) return default_flags();
			if (*fmtstr=='#') { f.alter = 1; ++fmtstr; goto cont; }
			if (*fmtstr=='0' && !f.minus) { f.zero = 1; ++fmtstr; goto cont; }
			if (*fmtstr=='-') { f.minus = 1; f.zero = 0; ++fmtstr; goto cont; }
			if (*fmtstr==' ' && !f.plus) { f.space = 1; ++fmtstr; goto cont; }
			if (*fmtstr=='+') { f.plus = 1; f.space = 0; ++fmtstr; goto cont; }
			{
				char const *const p = fmtstr;
				f.width = strtol(p, (char **)&fmtstr, 10); // ugly cast to char **
				f.width_enable = (p != fmtstr);
			}
			if (!*fmtstr) return default_flags();
			if (*fmtstr=='.') {
				f.prec_enable = 1;
				++fmtstr;
				if (!*fmtstr) return default_flags();
				char const *const p = fmtstr;
				int const n = strtol(p, (char **)&fmtstr, 10);
				f.precision = (n<=0) ? 0 : n;
				f.prec_enable = (p != fmtstr);
			}
		skipping:
			if (!*fmtstr) return default_flags();
			switch (*fmtstr) {
			case 'l':
			case 'L':
			case 'h':
			case 'q':
			case 'j':
			case 'z':
			case 't':
				++fmtstr;
				goto skipping;
			case 'c':
			case 's':
				f.character = 1;
				break;
			case 'i':
			case 'd':
			case 'u':
				break;
			case 'X':
				f.capital = 1;
			case 'x': // fallthrough
			case 'p':
				f.radix = 16;
				break;
			case 'o':
				f.radix = 8;
				break;
			case 'b':
				f.radix = 2;
				break;
			case 'E':
				f.capital = 1;
			case 'e': // fallthrough
				f.exponential = 1;
				break;
			case 'F':
				f.capital = 1;
			case 'f': // fallthrough
				f.fixed = 1;
				break;
			case 'G':
				f.capital = 1;
			case 'g': // fallthrough
				break;
			case 'A':
				f.capital = 1;
			case 'a': // fallthrough
				f.exponential = 1;
				f.radix = 16;
				break;
			}
			return f;
		}

		template <size_t N>
		inline void fill_str(char (&str)[N], char const c) { memset(str, c, N); }

		template <typename T>
		void integer_printer_helper(sink_fn_t &fn, T value, flags_t const &flags, bool inv) {
			char head[6] = { };
			char *phead = head;
			char buf[numeric_limits<T>::digits + 2];
			char *r = buf + sizeof(buf) - 1;
			fill_str(buf, '0');
			*r = 0;
			unsigned const prec = flags.prec_enable ? flags.precision : 0;

			if (value != 0 || !flags.prec_enable || prec != 0) {
				if (inv) *phead++ = '-';
				else if (flags.plus) *phead++ = '+';
				else if (flags.space) *phead++ = ' ';

				if (flags.alter) {
					if (flags.radix != 10) {
						*phead++ = '0';
						if (flags.radix == 16) *phead++ = flags.capital ? 'X' : 'x';
						if (flags.radix == 2) *phead++ = flags.capital ? 'B' : 'b';
					}
				}

				while (value) {
					char const c = hexstr[value%flags.radix];
					*--r = flags.capital ? static_cast<char>(toupper(c)) : c;
					value /= flags.radix;
				}
			}
			size_t const rl = strlen(r);
			size_t const wid = strlen(head) + (rl < prec ? prec : rl);
			if (!flags.minus) for (size_t n = wid; n < flags.width; ++n) fn(' ');
			fn(head);
			for (size_t n = rl; n < prec; ++n) fn('0');
			fn(r);
			if (flags.minus) for (size_t n = wid; n < flags.width; ++n) fn(' ');
		}

		template <typename T>
		inline bool is_negative_value(T const &value, typename enable_if< is_signed<T> >::type * = 0) { return value < 0; }
		template <typename T>
		inline bool is_negative_value(T const &, typename enable_if< is_unsigned<T> >::type * = 0) { return false; }
		template <typename T>
		inline typename make_unsigned<T>::type exact_abs(T const &value, typename enable_if< is_signed<T> >::type * = 0) { return abs(value); }
		template <typename T>
		inline T exact_abs(T const &value, typename enable_if< is_unsigned<T> >::type * = 0) { return value; }
		template <typename T>
		inline void integer_printer(sink_fn_t &fn, T const &value, flags_t const &flags) {
			typedef typename make_unsigned<T>::type unsigned_type;
			integer_printer_helper(fn, exact_abs(value), flags, is_negative_value(value));
		}

		template <typename T> struct is_character_type: bool_<false> { };
		template <> struct is_character_type<char>: bool_<true> { };
		template <> struct is_character_type<wchar_t>: bool_<true> { };
		template <typename T> struct is_boolean_type: bool_<false> { };
		template <> struct is_boolean_type<bool>: bool_<true> { };
		template <typename T> struct is_usual_integral_type: not_< or_< is_character_type<T>, is_boolean_type<T> > > { };
		template <typename T> struct is_usual_unsigned_type: and_< is_unsigned<T>, is_usual_integral_type<T> > { };
		template <typename T> struct is_usual_signed_type: and_< is_signed<T>, is_usual_integral_type<T> > { };

		template <typename T>
		inline void default_printer(sink_fn_t &fn, T const &value, flags_t const &flags, typename enable_if< is_usual_unsigned_type<T> >::type * = 0) {
			integer_printer<typename if_c<sizeof(T) <= sizeof(unsigned long), unsigned long, T>::type>(fn, value, flags);
		}
		template <typename T>
		inline void default_printer(sink_fn_t &fn, T const &value, flags_t const &flags, typename enable_if< is_usual_signed_type<T> >::type * = 0) {
			integer_printer<typename if_c<sizeof(T) <= sizeof(long), long, T>::type>(fn, value, flags);
		}
		template <typename T>
		inline void default_printer(sink_fn_t &fn, T const &value, flags_t const &flags, typename enable_if< is_pointer<T> >::type * = 0) {
			integer_printer<uintptr_t>(fn, reinterpret_cast<uintptr_t>(value), flags);
		}

		template <typename T>
		void float_printer(sink_fn_t &fn, T const &value, const flags_t &flags);
		template <typename T>
		inline void default_printer(sink_fn_t &fn, T const &value, flags_t const &flags, typename enable_if< is_floating_point<T> >::type * = 0) {
			float_printer<typename if_c<sizeof(T) <= sizeof(double), double, T>::type>(fn, value, flags);
		}

		template <typename T>
		inline void default_printer(sink_fn_t &fn, T const &value, flags_t const &flags, typename enable_if< is_boolean_type<T> >::type * = 0) {
			if (flags.alter) {
				char const *const s = value ? "true" : "false";
				size_t const l = strlen(s);
				if (flags.minus) fn(s);
				for (size_t n = l; n < flags.width; ++n) fn(' ');
				if (!flags.minus) fn(s);
			} else {
				if (!flags.minus) for (unsigned n = 1; n < flags.width; ++n) fn(' ');
				fn(value ? '1' : '0');
				if (flags.minus) for (unsigned n = 1; n < flags.width; ++n) fn(' ');
			}
		}

		template <typename T>
		inline void default_printer(sink_fn_t &fn, T const value, flags_t const &flags, typename enable_if< is_character_type<T> >::type * = 0) {
			if (flags.minus) fn(value);
			for (unsigned n = 1; n < flags.width; ++n) fn(' ');
			if (!flags.minus) fn(value);
		}

		template <typename T, typename baseT>
		struct is_c_string: is_convertible<T, baseT const *> { };
		template <typename charT>
		inline size_t gstrlen(charT const *const s);
		template <>
		inline size_t gstrlen<char>(char const *const s) { return strlen(s); }
#ifndef __CYGWIN__
		template <>
		inline size_t gstrlen<wchar_t>(wchar_t const *const s) { return wcslen(s); }
#endif
		template <typename T>
		inline void cstr_printer(sink_fn_t &fn, T const *const value, flags_t const &flags) {
			if (!flags.minus) fn(value);
			for (size_t n = gstrlen(value); n < flags.width; ++n) fn(' ');
			if (flags.minus) fn(value);
		}
		template <typename T>
		inline void default_printer(sink_fn_t &fn, T const &value, flags_t const &flags, typename enable_if< is_c_string<T, char> >::type * = 0) {
			cstr_printer<char>(fn, value, flags);
		}
		template <typename T>
		inline void default_printer(sink_fn_t &fn, T const &value, flags_t const &flags, typename enable_if< is_c_string<T, wchar_t> >::type * = 0) {
			cstr_printer<wchar_t>(fn, value, flags);
		}

	}

	template <typename T>
	struct fmt_t {
		fmt_t(T const &v, char const *const f): value(v), flags(detail::decode_flags(f)) { }
		fmt_t(T const &v, flags_t const &f): value(v), flags(f) { }
		template <typename Fn>
		void print(Fn &fn) const { detail::default_printer(fn, value, flags); }
		T const &value;
		flags_t const flags;
	};

	template <typename Fn>
	struct sink_t {
#ifdef BOOST_HAS_VARIADIC_TMPL
		void format() const { }
		template <typename A1, typename... Args>
		void format(A1 const &a1, Args const &...args) {
			fmt(a1).print(fn);
			format(args...);
		}
		template <typename A1, typename... Args>
		void format(fmt_t<A1> const &a1, Args const &...args) {
			a1.print(fn);
			format(args...);
		}
		template <typename A1, typename... Args>
		void format(fmt_t<A1> const &a1, Args const &...args) const { const_cast<sink_t *>(this)->format(a1, std::forward<Args>(args)...); }
		template <typename A1, typename... Args>
		void format(A1 const &a1, Args const &...args) const { const_cast<sink_t *>(this)->format(a1, std::forward<Args>(args)...); }
		template <typename... Args>
		sink_t(Args &&...args): fn(std::forward<Args>(args)...) { }
#else
		sink_t(): fn() { }
#define rARG(n) A##n &a##n
#define cARG(n) A##n const &a##n
#define ARG(r,t,n,e) BOOST_PP_COMMA_IF(n) BOOST_PP_EXPAND(BOOST_PP_CAT(e, ARG)(n))
#define FWD(z,n,t) a##n
#define CTOR_HELPER(n,arg_seq) \
	template <BOOST_PP_ENUM_PARAMS(BOOST_PP_EXPAND(BOOST_PP_SEQ_SIZE(arg_seq)), typename A)> \
		sink_t(BOOST_PP_SEQ_FOR_EACH_I(ARG, 0, arg_seq)): fn(BOOST_PP_ENUM_PARAMS(BOOST_PP_EXPAND(BOOST_PP_SEQ_SIZE(arg_seq)), a)) { }
#define MAKE_RC_SEQ(z,n,t) ((r)(c))
#define CTOR(z,n,t) \
	BOOST_PP_SEQ_FOR_EACH_PRODUCT(CTOR_HELPER, BOOST_PP_REPEAT(BOOST_PP_ADD(n,1),MAKE_RC_SEQ,0))
		BOOST_PP_REPEAT(6,CTOR,0)
		/* here generates
		  template <typename A0> sink_t(A0 &a0): fn(a0) { }
		  template <typename A0> sink_t(A0 const &a0): fn(a0) { }
		  template <typename A0, typename A1> sink_t(A0 &a0, A1 &a1): fn(a0, a1) { }
		  template <typename A0, typename A1> sink_t(A0 const &a0, A1 &a1): fn(a0, a1) { }
		  template <typename A0, typename A1> sink_t(A0 &a0, A1 const &a1): fn(a0, a1) { }
		  template <typename A0, typename A1> sink_t(A0 const &a0, A1 const &a1): fn(a0, a1) { }
		  ...
		*/
#undef CTOR
#undef MAKE_RC_SEQ
#undef CTOR_HELPER
#undef FWD
#undef ARG
#undef cARG
#undef rARG
#endif
		Fn fn;
		
		template <typename T>
		sink_t const &operator <<(fmt_t<T> const &v) {
			v.print(fn);
			return *this;
		}
		template <typename T>
		sink_t const &operator <<(T const &v) {
			fmt(v).print(fn);
			return *this;
		}
		template <typename T>
		sink_t const &operator <<(fmt_t<T> const &v) const { return const_cast<sink_t *>(this)->operator <<(v); }
		template <typename T>
		sink_t const &operator <<(T const &v) const { return const_cast<sink_t *>(this)->operator <<(v); }
	};
	typedef sink_t<detail::sink_strbuf_fn_t> sink_strbuf;
}

#include "ntfmt_float.hpp"

#endif
