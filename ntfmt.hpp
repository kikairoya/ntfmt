#ifndef NTFMT_HPP_
#define NTFMT_HPP_

#include "ntfmt_fwd.hpp"

#include <string.h>
#include <wchar.h>
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

namespace ntfmt {
	inline flags_t const default_flags() {
		flags_t f;
		memset(&f, 0, sizeof(f));
		f.radix = 10;
		return f;
	}
	inline packed_flags_t const default_packed_flags() {
		packed_flags_t f;
		memset(&f, 0, sizeof(f));
		f.radix = 10;
		return f;
	}
	inline unpacked_flags_t const default_unpacked_flags() {
		unpacked_flags_t f;
		memset(&f, 0, sizeof(f));
		f.radix = 10;
		return f;
	}

	namespace detail {
#ifndef BOOST_DINKUMWARE_STDLIB
		using std::abs;
#endif
#ifdef __CYGWIN__
		template <typename IntT>
		inline IntT abs(IntT v) { return std::abs(v); }
		template <>
		inline long long abs(long long const v) { return v < 0 ? -v : v; }
		template <>
		inline unsigned long long abs(unsigned long long const v) { return v; }
#endif
		using std::numeric_limits;
		using std::char_traits;
		using boost::mpl::and_;
		using boost::mpl::or_;
		using boost::mpl::not_;
		using boost::mpl::if_c;
		using boost::enable_if;
		using boost::integral_constant;
		using boost::true_type;
		using boost::false_type;
		using boost::is_convertible;
		using boost::is_integral;
		using boost::is_unsigned;
		using boost::is_signed;
		using boost::is_floating_point;
		using boost::is_pointer;
		using boost::make_unsigned;

		template <typename T1, typename T2>
		inline T1 ref_max(T1 const &v1, T2 const &v2) { return (v1 < v2) ? v2 : v1; }
		template <typename T1, typename T2>
		inline T1 ref_min(T1 const &v1, T2 const &v2) { return (v2 < v1) ? v2 : v1; }
		template <typename T1, typename T2, typename T3>
		inline T2 ref_clip(T1 const &l, T2 const &v, T3 const &h) { return ref_max(ref_min(v, h), l); }
		template <typename T, size_t N>
		inline T *array_begin(T (&a)[N]) { return &a[0]; }
		template <typename T, size_t N>
		inline T *array_end(T (&a)[N]) { return &a[N]; }
		template <typename T>
		struct array_elements;
		template <typename T, size_t N>
		struct array_elements<T [N]>: integral_constant<size_t, N> { };
		template <typename charT> struct hexstr;
		template <>
		struct hexstr<char> {
			static char const (&str(bool const capital))[18] {
				if (capital) return "0123456789ABCDEFG";
				return "0123456789abcdefg";
			}
		};
		template <>
		struct hexstr<wchar_t> {
			static wchar_t const (&str(bool const capital))[18] {
				if (capital) return L"0123456789ABCDEFG";
				return L"0123456789abcdefg";
			}
		};
		template <typename charT, typename IntT>
		inline charT to_hexstr(IntT const v, unsigned const base, bool const capital, typename enable_if< is_integral<IntT> >::type * = 0) {
			return hexstr<charT>::str(capital)[v%base];
		}
		template <typename charT>
		inline int from_hexstr(charT const c, unsigned base, bool const capital) {
			charT const *const str = hexstr<charT>::str(capital);
			return static_cast<int>(std::find(str, str+base, c) - str);
		}

		template <typename charT, char C, wchar_t W>
		struct char_literal;
		template <char C, wchar_t W>
		struct char_literal<char, C, W>: integral_constant<char, C> { };
		template <char C, wchar_t W>
		struct char_literal<wchar_t, C, W>: integral_constant<wchar_t, W> { };
#define NTFMT_CH_LIT(c) ::ntfmt::detail::char_literal<charT, c, L##c>::value
#define NTFMT_CHR_ZERO NTFMT_CH_LIT('0')
#define NTFMT_CHR_SPACE NTFMT_CH_LIT(' ')
#define NTFMT_CHR_MINUS NTFMT_CH_LIT('-')
#define NTFMT_CHR_PLUS NTFMT_CH_LIT('+')
#define NTFMT_CHR_DOT NTFMT_CH_LIT('.')

		template <typename charT>
		inline size_t gstrlen(charT const *const s) { return char_traits<charT>::length(s); }

		template <typename charT>
		inline bool gisdigit(charT const c) { return static_cast<unsigned>(from_hexstr(c, 10, false)) < 10; }
		template <typename charT>
		int extract_decimal_int(charT const *nptr, charT const **endp) {
			int ret = 0;
			while (gisdigit(*nptr)) {
				ret *= 10;
				ret += from_hexstr(*nptr++, 10, false);
			}
			*endp = nptr;
			return ret;
		}

		template <typename charT>
		flags_t const decode_flags(charT const *fmtstr) {
			flags_t const def_flags = default_flags();
			unpacked_flags_t f = default_unpacked_flags();
			if (!fmtstr || *fmtstr++!=NTFMT_CH_LIT('%')) return f;
		cont:
			if (!*fmtstr) return def_flags;
			if (*fmtstr==NTFMT_CH_LIT('#')) { f.alter = 1; ++fmtstr; goto cont; }
			if (*fmtstr==NTFMT_CHR_ZERO && !f.minus) { f.zero = 1; ++fmtstr; goto cont; }
			if (*fmtstr==NTFMT_CHR_MINUS) { f.minus = 1; f.zero = 0; ++fmtstr; goto cont; }
			if (*fmtstr==NTFMT_CHR_SPACE && !f.plus) { f.space = 1; ++fmtstr; goto cont; }
			if (*fmtstr==NTFMT_CHR_PLUS) { f.plus = 1; f.space = 0; ++fmtstr; goto cont; }
			{
				charT const *const p = fmtstr;
				f.width = extract_decimal_int(p, &fmtstr);
				f.width_enable = (p != fmtstr);
			}
			if (!*fmtstr) return def_flags;
			if (*fmtstr==NTFMT_CHR_DOT) {
				f.prec_enable = 1;
				++fmtstr;
				if (!*fmtstr) return def_flags;
				charT const *const p = fmtstr;
				int const n = extract_decimal_int(p, &fmtstr);
				f.precision = (n<=0) ? 0 : n;
				f.prec_enable = (p != fmtstr);
			}
		skipping:
			if (!*fmtstr) return def_flags;
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
			case NTFMT_CH_LIT('I'):
				if ((fmtstr[1]==NTFMT_CH_LIT('6') && fmtstr[2]==NTFMT_CH_LIT('4')) ||
					(fmtstr[1]==NTFMT_CH_LIT('3') && fmtstr[2]==NTFMT_CH_LIT('2'))) fmtstr += 3;
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
			if (f.minus) f.zero = 0;
			if (f.plus) f.space = 0;
			return f;
		}

		template <typename charT>
		int fill_chr_to(sink_fn_t<charT> &fn, charT const c, int N) { int n; for (n = 0; n < N && fn(c) >= 0; ++n) ; return n; }
		template <typename charT, typename T>
		void integer_printer_helper(sink_fn_t<charT> &fn, T value, flags_t const &flags, bool inv) {
			charT head[6] = { };
			charT *phead = head;
			charT buf[numeric_limits<T>::digits + 2];
			charT *r = array_end(buf) - 1;
			std::fill(array_begin(buf), array_end(buf), NTFMT_CHR_ZERO);
			*r = 0;
			unsigned const prec = flags.prec_enable ? flags.precision : 1;

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
					*--r = to_hexstr<charT>(value, flags.radix, flags.capital);
					value /= flags.radix;
				}
			}
			size_t const rl = gstrlen(r);
			size_t const wid = gstrlen(head) + ref_max(rl, prec);
			if (!flags.minus && !flags.zero) fill_chr_to(fn, NTFMT_CHR_SPACE, flags.width - wid);
			fn(head);
			if (flags.zero) fill_chr_to(fn, NTFMT_CHR_ZERO, flags.width - wid);
			fill_chr_to(fn, NTFMT_CHR_ZERO, prec - rl);
			fn(r);
			if (flags.minus) fill_chr_to(fn, NTFMT_CHR_SPACE, flags.width - wid);
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
		template <typename T, typename U> struct select_larger_type: if_c<(sizeof(T)<sizeof(U)), U, T> { };
		template <typename T, typename baseT> struct is_c_string: is_convertible<T, baseT const *> { };

		template <typename charT, typename T>
		inline void default_printer(sink_fn_t<charT> &fn, T const &value, flags_t const &flags, typename enable_if< is_usual_unsigned_type<T> >::type * = 0) {
			integer_printer<charT, typename select_larger_type<unsigned long, T>::type>(fn, value, flags);
		}
		template <typename charT, typename T>
		inline void default_printer(sink_fn_t<charT> &fn, T const &value, flags_t const &flags, typename enable_if< is_usual_signed_type<T> >::type * = 0) {
			integer_printer<charT, typename select_larger_type<long, T>::type>(fn, value, flags);
		}
		template <typename charT, typename T>
		inline void default_printer(sink_fn_t<charT> &fn, T const &value, flags_t const &flags, typename enable_if< and_< is_pointer<T>, not_< is_c_string<T, charT> > > >::type * = 0) {
			integer_printer<charT, uintptr_t>(fn, reinterpret_cast<uintptr_t>(value), flags);
		}

		template <typename charT, typename T>
		void float_printer(sink_fn_t<charT> &fn, T const &value, const flags_t &flags);
		template <typename charT, typename T>
		inline void default_printer(sink_fn_t<charT> &fn, T const &value, flags_t const &flags, typename enable_if< is_floating_point<T> >::type * = 0) {
			float_printer<charT, typename select_larger_type<double, T>::type>(fn, value, flags);
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
				if (flags.minus) fn(s);
				fill_chr_to(fn, NTFMT_CHR_SPACE, flags.width - gstrlen(s));
				if (!flags.minus) fn(s);
			} else {
				default_printer(fn, static_cast<unsigned long>(value), flags);
			}
		}

		template <typename charT>
		inline void default_printer(sink_fn_t<charT> &fn, charT const value, flags_t const &flags, typename enable_if< is_character_type<charT> >::type * = 0) {
			if (flags.minus) fn(value);
			fill_chr_to(fn, NTFMT_CHR_SPACE, flags.width - 1);
			if (!flags.minus) fn(value);
		}

		template <typename charT, typename T>
		inline void default_printer(sink_fn_t<charT> &fn, T const &value, flags_t const &flags, typename enable_if< is_c_string<T, charT> >::type * = 0) {
			if (!flags.minus) fn(value);
			fill_chr_to(fn, NTFMT_CHR_SPACE, flags.width - gstrlen(value));
			if (flags.minus) fn(value);
		}
	}

	template <typename charT, typename T>
	inline void ntfmt_printer(sink_fn_t<charT> &fn, T const &value, flags_t const &flags) {
		detail::default_printer(fn, value, flags);
	}

	template <typename T>
	struct fmt_t {
		fmt_t(T const &v, char const *const f): value(v), flags(detail::decode_flags(f)) { }
		fmt_t(T const &v, wchar_t const *const f): value(v), flags(detail::decode_flags(f)) { }
		fmt_t(T const &v, flags_t const &f): value(v), flags(f) { }
		template <typename charT>
		void print(sink_fn_t<charT> &fn) const { ntfmt_printer(fn, value, flags); }
	private:
		T const &value;
		flags_t const flags;
	};

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

	typedef sink_strbuf_fn_t<char> sink_strbuf;
	typedef sink_strbuf_fn_t<wchar_t> sink_wstrbuf;
}

#ifndef NTFMT_DISABLE_FLOAT
#include "ntfmt_float.hpp"
#endif

#endif
