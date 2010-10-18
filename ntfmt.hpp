#ifndef NTFMT_HPP_
#define NTFMT_HPP_

#include "ntfmt_fwd.hpp"

#include <string.h>
#include <ctype.h>
#include <wctype.h>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <limits>
#include <utility>
#include <boost/utility/enable_if.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/logical.hpp>
#include <boost/type_traits/integral_constant.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/type_traits/is_unsigned.hpp>
#include <boost/type_traits/is_signed.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/type_traits/make_unsigned.hpp>

#include <boost/preprocessor.hpp>

namespace ntfmt {
	namespace detail {
#ifndef BOOST_DINKUMWARE_STDLIB
		using std::abs;
#endif
		using std::numeric_limits;
		using boost::mpl::and_;
		using boost::mpl::or_;
		using boost::mpl::not_;
		using boost::mpl::if_c;
		using boost::enable_if;
		using boost::integral_constant;
		using boost::true_type;
		using boost::false_type;
		using boost::is_convertible;
		using boost::is_unsigned;
		using boost::is_signed;
		using boost::is_floating_point;
		using boost::is_pointer;
		using boost::make_unsigned;

		template <typename T, size_t N>
		inline T *array_begin(T (&a)[N]) { return &a[0]; }
		template <typename T, size_t N>
		inline T *array_end(T (&a)[N]) { return &a[N]; }
		template <typename T>
		struct array_elements;
		template <typename T, size_t N>
		struct array_elements<T [N]>: integral_constant<size_t, N> { };
		template <typename charT> struct hexstr;
		template <> struct hexstr<char> { static char const (&str())[18] { return "0123456789abcdefg"; } };
		template <> struct hexstr<wchar_t> { static wchar_t const (&str())[18] { return L"0123456789abcdefg"; } };
		template <typename charT>
		inline charT to_hexstr(unsigned const v) { return hexstr<charT>::str()[v]; }
		template <typename charT>
		inline ptrdiff_t from_hexstr(charT c) {
			typedef hexstr<charT> hex;
			return std::lower_bound(array_begin(hex::str()), array_end(hex::str()), c) - array_begin(hex::str());
		}

		template <typename charT>
		inline long gstrtol(charT const *, charT const **, int);
		template <>
		inline long gstrtol<char>(char const *nptr, char const **endp, int base) { return strtol(nptr, (char **)endp, base); }
		template <>
		inline long gstrtol<wchar_t>(wchar_t const *nptr, wchar_t const **endp, int base) { return wcstol(nptr, (wchar_t **)endp, base); }

		template <typename charT>
		inline size_t gstrlen(charT const *const s);
		template <>
		inline size_t gstrlen<char>(char const *const s) { return strlen(s); }
		template <>
		inline size_t gstrlen<wchar_t>(wchar_t const *const s) { return wcslen(s); }

		template <typename charT>
		inline charT gtoupper(charT const c);
		template <>
		inline char gtoupper<char>(char const c) { return static_cast<char>(toupper(c)); }
		template <>
		inline wchar_t gtoupper<wchar_t>(wchar_t const c) { return towupper(c); }

		template <typename charT, char C, wchar_t W>
		struct char_literal;
		template <char C, wchar_t W>
		struct char_literal<char, C, W>: integral_constant<char, C> { };
		template <char C, wchar_t W>
		struct char_literal<wchar_t, C, W>: integral_constant<wchar_t, W> { };
#define NTFMT_CH_LIT(c) char_literal<charT, c, L##c>::value
#define NTFMT_CHR_ZERO NTFMT_CH_LIT('0')
#define NTFMT_CHR_SPACE NTFMT_CH_LIT(' ')
#define NTFMT_CHR_MINUS NTFMT_CH_LIT('-')
#define NTFMT_CHR_PLUS NTFMT_CH_LIT('+')
#define NTFMT_CHR_DOT NTFMT_CH_LIT('.')

		template <typename charT>
		struct sink_strbuf_fn_t: sink_fn_t<charT> {
			template <size_t N>
			sink_strbuf_fn_t(charT (*const &buf)[N]): buf(*buf), p(*buf), size(N) { }
			sink_strbuf_fn_t(charT *const buf, size_t const size): buf(buf), p(buf), size(size) { }
			int operator ()(charT c) { if (p < buf+size-1) { *p++ = c; *p = 0; return c; } return -1; }
		private:
			charT *const buf;
			charT *p;
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
			if (!fmtstr || *fmtstr++!=NTFMT_CH_LIT('%')) return f;
		cont:
			if (!*fmtstr) return default_flags();
			if (*fmtstr==NTFMT_CH_LIT('#')) { f.alter = 1; ++fmtstr; goto cont; }
			if (*fmtstr==NTFMT_CHR_ZERO && !f.minus) { f.zero = 1; ++fmtstr; goto cont; }
			if (*fmtstr==NTFMT_CHR_MINUS) { f.minus = 1; f.zero = 0; ++fmtstr; goto cont; }
			if (*fmtstr==NTFMT_CHR_SPACE && !f.plus) { f.space = 1; ++fmtstr; goto cont; }
			if (*fmtstr==NTFMT_CHR_PLUS) { f.plus = 1; f.space = 0; ++fmtstr; goto cont; }
			{
				charT const *const p = fmtstr;
				f.width = static_cast<int>(gstrtol(p, &fmtstr, 10));
				f.width_enable = (p != fmtstr);
			}
			if (!*fmtstr) return default_flags();
			if (*fmtstr==NTFMT_CHR_DOT) {
				f.prec_enable = 1;
				++fmtstr;
				if (!*fmtstr) return default_flags();
				charT const *const p = fmtstr;
				int const n = static_cast<int>(gstrtol(p, &fmtstr, 10));
				f.precision = (n<=0) ? 0 : n;
				f.prec_enable = (p != fmtstr);
			}
		skipping:
			if (!*fmtstr) return default_flags();
			switch (*fmtstr) {
			case NTFMT_CH_LIT('l'):
			case NTFMT_CH_LIT('L'):
			case NTFMT_CH_LIT('h'):
			case NTFMT_CH_LIT('q'):
			case NTFMT_CH_LIT('j'):
			case NTFMT_CH_LIT('z'):
			case NTFMT_CH_LIT('t'):
				++fmtstr;
				goto skipping;
			case NTFMT_CH_LIT('c'):
			case NTFMT_CH_LIT('s'):
				f.character = 1;
				break;
			case NTFMT_CH_LIT('i'):
			case NTFMT_CH_LIT('d'):
			case NTFMT_CH_LIT('u'):
				break;
			case NTFMT_CH_LIT('X'):
				f.capital = 1;
			case NTFMT_CH_LIT('x'): // fallthrough
			case NTFMT_CH_LIT('p'):
				f.radix = 16;
				break;
			case NTFMT_CH_LIT('o'):
				f.radix = 8;
				break;
			case NTFMT_CH_LIT('b'):
				f.radix = 2;
				break;
			case NTFMT_CH_LIT('E'):
				f.capital = 1;
			case NTFMT_CH_LIT('e'): // fallthrough
				f.exponential = 1;
				break;
			case NTFMT_CH_LIT('F'):
				f.capital = 1;
			case NTFMT_CH_LIT('f'): // fallthrough
				f.fixed = 1;
				break;
			case NTFMT_CH_LIT('G'):
				f.capital = 1;
			case NTFMT_CH_LIT('g'): // fallthrough
				break;
			case NTFMT_CH_LIT('A'):
				f.capital = 1;
			case NTFMT_CH_LIT('a'): // fallthrough
				f.exponential = 1;
				f.radix = 16;
				break;
			}
			return f;
		}

		template <typename charT, typename T>
		void integer_printer_helper(sink_fn_t<charT> &fn, T value, flags_t const &flags, bool inv) {
			charT head[6] = { };
			charT *phead = head;
			charT buf[numeric_limits<T>::digits + 2];
			charT *r = array_end(buf) - 1;
			std::fill(array_begin(buf), array_end(buf), NTFMT_CHR_ZERO);
			*r = 0;
			unsigned const prec = flags.prec_enable ? flags.precision : 0;

			if (value != 0 || !flags.prec_enable || prec != 0) {
				if (inv) *phead++ = NTFMT_CHR_MINUS;
				else if (flags.plus) *phead++ = NTFMT_CHR_PLUS;
				else if (flags.space) *phead++ = NTFMT_CHR_SPACE;

				if (flags.alter) {
					if (flags.radix != 10) {
						*phead++ = NTFMT_CHR_ZERO;
						if (flags.radix == 16) *phead++ = flags.capital ? NTFMT_CH_LIT('X') : NTFMT_CH_LIT('x');
						if (flags.radix == 2) *phead++ = flags.capital ? NTFMT_CH_LIT('B') : NTFMT_CH_LIT('b');
					}
				}

				while (value) {
					charT const c = to_hexstr<charT>(static_cast<unsigned>(value%flags.radix));
					*--r = flags.capital ? gtoupper(c) : c;
					value /= flags.radix;
				}
			}
			size_t const rl = gstrlen(r);
			size_t const wid = gstrlen(head) + (rl < prec ? prec : rl);
			if (!flags.minus) for (size_t n = wid; n < flags.width; ++n) fn(NTFMT_CHR_SPACE);
			fn(head);
			for (size_t n = rl; n < prec; ++n) fn(NTFMT_CHR_ZERO);
			fn(r);
			if (flags.minus) for (size_t n = wid; n < flags.width; ++n) fn(NTFMT_CHR_SPACE);
		}

		template <typename T>
		inline bool is_negative_value(T const &value, typename enable_if< is_signed<T> >::type * = 0) { return value < 0; }
		template <typename T>
		inline bool is_negative_value(T const &, typename enable_if< is_unsigned<T> >::type * = 0) { return false; }
		template <typename T>
		inline typename make_unsigned<T>::type exact_abs(T const &value, typename enable_if< is_signed<T> >::type * = 0) { return abs(value); }
		template <typename T>
		inline T exact_abs(T const &value, typename enable_if< is_unsigned<T> >::type * = 0) { return value; }
		template <typename charT, typename T>
		inline void integer_printer(sink_fn_t<charT> &fn, T const &value, flags_t const &flags) {
			typedef typename make_unsigned<T>::type unsigned_type;
			integer_printer_helper(fn, exact_abs(value), flags, is_negative_value(value));
		}

		template <typename T> struct is_character_type: false_type { };
		template <> struct is_character_type<char>: true_type { };
		template <> struct is_character_type<wchar_t>: true_type { };
		template <typename T> struct is_boolean_type: false_type { };
		template <> struct is_boolean_type<bool>: true_type { };
		template <typename T> struct is_usual_integral_type: not_< or_< is_character_type<T>, is_boolean_type<T> > > { };
		template <typename T> struct is_usual_unsigned_type: and_< is_unsigned<T>, is_usual_integral_type<T> > { };
		template <typename T> struct is_usual_signed_type: and_< is_signed<T>, is_usual_integral_type<T> > { };

		template <typename charT, typename T>
		inline void default_printer(sink_fn_t<charT> &fn, T const &value, flags_t const &flags, typename enable_if< is_usual_unsigned_type<T> >::type * = 0) {
			integer_printer<charT, typename if_c<sizeof(T) <= sizeof(unsigned long), unsigned long, T>::type>(fn, value, flags);
		}
		template <typename charT, typename T>
		inline void default_printer(sink_fn_t<charT> &fn, T const &value, flags_t const &flags, typename enable_if< is_usual_signed_type<T> >::type * = 0) {
			integer_printer<charT, typename if_c<sizeof(T) <= sizeof(long), long, T>::type>(fn, value, flags);
		}
		template <typename charT, typename T>
		inline void default_printer(sink_fn_t<charT> &fn, T const &value, flags_t const &flags, typename enable_if< is_pointer<T> >::type * = 0) {
			integer_printer<charT, uintptr_t>(fn, reinterpret_cast<uintptr_t>(value), flags);
		}

		template <typename charT, typename T>
		void float_printer(sink_fn_t<charT> &fn, T const &value, const flags_t &flags);
		template <typename charT, typename T>
		inline void default_printer(sink_fn_t<charT> &fn, T const &value, flags_t const &flags, typename enable_if< is_floating_point<T> >::type * = 0) {
			float_printer<charT, typename if_c<sizeof(T) <= sizeof(double), double, T>::type>(fn, value, flags);
		}

		template <typename charT> struct bool_str;
		template <>
		struct bool_str<char> {
			static char const *str(bool const b) {
				static char const true_str[] = "true";
				static char const false_str[] = "false";
				return b ? true_str : false_str;
			}
		};
		template <>
		struct bool_str<wchar_t> {
			static wchar_t const *str(bool const b) {
				static wchar_t const true_str[] = L"true";
				static wchar_t const false_str[] = L"false";
				return b ? true_str : false_str;
			}
		};
		template <typename charT, typename T>
		inline void default_printer(sink_fn_t<charT> &fn, T const &value, flags_t const &flags, typename enable_if< is_boolean_type<T> >::type * = 0) {
			if (flags.alter) {
				charT const *const s = bool_str<charT>::str(value);
				size_t const l = gstrlen(s);
				if (flags.minus) fn(s);
				for (size_t n = l; n < flags.width; ++n) fn(NTFMT_CHR_SPACE);
				if (!flags.minus) fn(s);
			} else {
				default_printer(fn, static_cast<unsigned>(value), flags);
			}
		}

		template <typename charT>
		inline void default_printer(sink_fn_base &fn, charT const value, flags_t const &flags, typename enable_if< is_character_type<charT> >::type * = 0) {
			if (flags.minus) fn(value);
			for (unsigned n = 1; n < flags.width; ++n) fn(NTFMT_CHR_SPACE);
			if (!flags.minus) fn(value);
		}

		template <typename T, typename baseT>
		struct is_c_string: is_convertible<T, baseT const *> { };
		template <typename charT, typename T>
		inline void default_printer(sink_fn_t<charT> &fn, T const &value, flags_t const &flags, typename enable_if< is_c_string<T, charT> >::type * = 0) {
			if (!flags.minus) fn(value);
			for (size_t n = gstrlen(value); n < flags.width; ++n) fn(NTFMT_CHR_SPACE);
			if (flags.minus) fn(value);
		}

	}

	template <typename T>
	struct fmt_t {
		fmt_t(T const &v, char const *const f): value(v), flags(detail::decode_flags(f)) { }
		fmt_t(T const &v, wchar_t const *const f): value(v), flags(detail::decode_flags(f)) { }
		fmt_t(T const &v, flags_t const &f): value(v), flags(f) { }
		template <typename Fn>
		void print(Fn &fn) const { detail::default_printer(fn, value, flags); }
	private:
		T const &value;
		flags_t const flags;
	};

	template <typename Fn>
	struct sink_t {
#ifdef BOOST_HAS_VARIADIC_TMPL
		sink_t &format() { return *this; }
		sink_t const &format() const { return *this; }
		template <typename A1, typename... Args>
		sink_t &format(A1 const &a1, Args const &...args) {
			fmt(a1).print(fn);
			return format(args...);
		}
		template <typename A1, typename... Args>
		sink_t &format(fmt_t<A1> const &a1, Args const &...args) {
			a1.print(fn);
			return format(args...);
		}
		template <typename A1, typename... Args>
		sink_t const &format(fmt_t<A1> const &a1, Args const &...args) const { return const_cast<sink_t *>(this)->format(a1).format(std::forward<Args>(args)...); }
		template <typename A1, typename... Args>
		sink_t const &format(A1 const &a1, Args const &...args) const { return const_cast<sink_t *>(this)->format(a1).format(std::forward<Args>(args)...); }
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
		BOOST_PP_REPEAT(6,CTOR,0);
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
		template <typename T>
		sink_t &operator <<(fmt_t<T> const &v) {
			v.print(fn);
			return *this;
		}
		template <typename T>
		sink_t &operator <<(T const &v) {
			fmt(v).print(fn);
			return *this;
		}
		template <typename T>
		sink_t const &operator <<(fmt_t<T> const &v) const { return const_cast<sink_t *>(this)->operator <<(v); }
		template <typename T>
		sink_t const &operator <<(T const &v) const { return const_cast<sink_t *>(this)->operator <<(v); }

	private:
		Fn fn;
	};
	typedef sink_t< detail::sink_strbuf_fn_t<char> > sink_strbuf;
}

#include "ntfmt_float.hpp"

#endif
