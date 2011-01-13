#ifndef NTFMT_FLOAT_HPP_
#define NTFMT_FLOAT_HPP_

#include "ntfmt_fwd.hpp"
#include "ntfmt.hpp"

#include <string.h>
#include <float.h>
#include <cmath>
#include <boost/integer.hpp>
#include <boost/type_traits/integral_promotion.hpp>

namespace ntfmt {
	namespace detail {
#ifdef BOOST_DINKUMWARE_STDLIB
		using ::fabs;
		using ::floor;
		using ::ceil;
		using ::fmod;
		using ::log;
		using ::log10;
		using ::pow;
		inline bool isinf(double d) { return !::_finite(d); }
		inline bool isinf(long double d) { return !::_finite((double)d); }
		inline bool isnan(double d) { return !!::_isnan(d); }
		inline bool isnan(long double d) { return !!::_isnan((double)d); }
		inline double nextafter(double x, double y) { return ::_nextafter(x, y); }
		inline long double nextafter(long double x, long double y) { return ::_nextafter((double)x, (double)y); }
		inline double copysign(double x) { return ::_copysign(x); }
		inline long double copysign(long double x) { return ::_copysign((double)x); }
#else
		using std::fabs;
		using std::floor;
		using std::ceil;
		using std::isinf;
		using std::isnan;
		using std::fmod;
		using std::log;
		using std::log10;
		using std::pow;
		using ::copysign;
		using ::nextafter;
#endif

		template <typename charT>
		inline charT *gstrncpy(charT *const, charT const *const, size_t);
		template <>
		inline char *gstrncpy<char>(char *const dst, char const *const src, size_t lim) { return strncpy(dst, src, lim); }
		template <>
		inline wchar_t *gstrncpy<wchar_t>(wchar_t *const dst, wchar_t const *const src, size_t lim) { return wcsncpy(dst, src, lim); }

		template <typename T>
		inline T get_predecessor(T const &v) { return nextafter(v, -numeric_limits<T>::infinity()); }
		template <typename T>
		inline T get_successor(T const &v) { return nextafter(v, +numeric_limits<T>::infinity()); }

		template <typename charT, typename FloatT>
		inline charT to_hexstr(FloatT const v, unsigned const base, bool const capital, typename enable_if< is_floating_point<FloatT> >::type * = 0) {
			return hexstr<charT>::str(capital)[static_cast<unsigned>(fmod(v, static_cast<FloatT>(base)))];
		}

		template <typename charT>
		inline void inc_strnum(charT *const strnum, unsigned const base, unsigned const col, bool const capital) {
			if (strnum[col]++ == to_hexstr<charT>(base-1, base, capital)) {
				strnum[col] = to_hexstr<charT>(0, base, capital);
				inc_strnum(strnum, base, col - 1, capital);
			}
		}

		template <typename T>
		inline T ipow(unsigned const base, int e) {
			return pow(static_cast<T>(base), e);
		}

		template <typename T>
		inline int ilog(unsigned const base, T v) {
			if (isinf(v)) return ilog(base, std::numeric_limits<T>::max());
			if (v==0) return 0;
			return static_cast<int>(floor( (base==10) ? log10(v) : (log(v)/log(static_cast<T>(base))) )) + 1;
		}

		template <typename T>
		struct dtoa_traits {
			typedef typename boost::int_max_value_t<numeric_limits<T>::max_exponent>::least higher_type;
			typedef typename boost::int_min_value_t<numeric_limits<T>::min_exponent>::least lower_type;
			typedef typename boost::integral_promotion<typename select_larger_type<higher_type, lower_type>::type>::type return_type;
		};
		template <typename charT, typename T, size_t N>
		typename dtoa_traits<T>::return_type dtoa(charT (&out)[N], T const v, int const pr, flags_t const &flags) {
			typedef typename dtoa_traits<T>::return_type return_type;
			unsigned const base = flags.radix;
			bool const capital = flags.capital;
			T const value = fabs(v);
			int const prec = ref_clip(0, pr, numeric_limits<T>::max_exponent);
			return_type const k = ilog(base, get_successor(value));
			
			out[0] = NTFMT_CHR_ZERO;
			{
				T q = (k>0) ? (value / ipow<T>(base, k-1)) : (value * ipow<T>(base, 1-k));

				charT const *const end = array_end(out) - 1;
				for (charT *ptr = array_begin(out) + 1; ptr < end; ++ptr) {
					*ptr = to_hexstr<charT>(q, base, capital);
					q = fmod(q, base) * base;
				}
			}
			if (static_cast<unsigned>(from_hexstr(out[prec+1], base, capital)) >= base/2) inc_strnum(out, base, prec, capital);

			if (out[0] == NTFMT_CHR_ZERO) {
				std::copy(out+1, out+prec+1, out);
				out[prec] = 0;
				return k;
			}
			out[prec+1] = 0;
			return k+1;
		}
		template <typename charT> struct nan_str;
		template <>
		struct nan_str<char> {
			static char const (&str(bool const capital))[4] {
				static char const s[][4] = { "nan", "NAN" };
				return s[capital];
			}
		};
		template <>
		struct nan_str<wchar_t> {
			static wchar_t const (&str(bool const capital))[4] {
				static wchar_t const s[][4] = { L"nan", L"NAN" };
				return s[capital];
			}
		};
		template <typename charT> struct inf_str;
		template <>
		struct inf_str<char> {
			static char const (&str(bool const capital))[4] {
				static char const s[][4] = { "inf", "INF" };
				return s[capital];
			}
		};
		template <>
		struct inf_str<wchar_t> {
			static wchar_t const (&str(bool const capital))[4] {
				static wchar_t const s[][4] = { L"inf", L"INF" };
				return s[capital];
			}
		};
		template <typename charT, typename T>
		void float_printer(sink_fn_t<charT> &fn, T const &value, const flags_t &flags) {
			using namespace std;
			if (ntfmt_unlikely(isnan(value))) {
				fn(nan_str<charT>::str(flags.capital));
				return;
			}
			charT head[6] = { };
			charT *phead = head;
			if (copysign(static_cast<T>(1), value) < 0) *phead++ = NTFMT_CHR_MINUS;
			else if (flags.plus) *phead++ = NTFMT_CHR_PLUS;
			else if (flags.space) *phead++ = NTFMT_CHR_SPACE;
			if (ntfmt_unlikely(isinf(value))) {
				fn(head);
				fn(inf_str<charT>::str(flags.capital));
				return;
			}
			int const prec = flags.prec_enable ? flags.precision : 6;
			int const l = ilog(flags.radix, fabs(value));
			bool const exponential = flags.exponential || ( !flags.fixed && ((l < -4) || (l > prec)));
			charT buf[numeric_limits<T>::max_exponent + 8] = { };
			if (exponential) {
				int const e = dtoa<charT, T>(buf, value, prec + flags.exponential, flags);
				if (!flags.exponential && !flags.alter) {
					charT *p = buf + gstrlen(buf) - 1;
					while (*p == NTFMT_CHR_ZERO) *p-- = 0; 
				}
				*phead++ = buf[0];
				buf[0] = buf[1] && (prec!=0 || flags.alter) ? NTFMT_CHR_DOT : 0;

				size_t const wid = gstrlen(head) + gstrlen(buf) + 5;
				if (!flags.minus) fill_chr_to(fn, NTFMT_CHR_SPACE, flags.width - wid);
				fn(head);
				fn(buf);
				fn(flags.capital ? NTFMT_CH_LIT('E') : NTFMT_CH_LIT('e'));
				flags_t ff = default_flags();
				ff.zero = 1;
				ff.prec_enable = 1;
				ff.precision = 3;
				ff.plus = 1;
				integer_printer(fn, static_cast<long>(e-1), ff);
				if (flags.minus) fill_chr_to(fn, NTFMT_CHR_SPACE, flags.width - wid);
			} else {
				int const e = dtoa<charT, T>(buf, value, prec + (flags.fixed ? l : 0), flags);
				if (e < 1) {
					*phead++ = NTFMT_CHR_ZERO;
					if (prec!=0) *phead++ = NTFMT_CHR_DOT;
					if (e <= -prec) buf[0] = 0;
				} else {
					if (prec!=0) {
						charT *const end = &buf[gstrlen(buf)]+1;
						std::rotate(&buf[e], end, end+1);
						buf[e] = NTFMT_CHR_DOT;
					}
					if (!flags.fixed && !flags.alter) {
						charT *p = buf + gstrlen(buf) - 1;
						while (*p == NTFMT_CHR_ZERO) *p-- = 0;
						if (*p == NTFMT_CHR_DOT) *p-- = 0;
					}
				}
				int const lim = e < -prec ? prec : -e;
				size_t const wid = gstrlen(head) + gstrlen(buf) + lim + (e>=1);
				
				if (!flags.minus) fill_chr_to(fn, NTFMT_CHR_SPACE, flags.width - wid);
				fn(head);
				fill_chr_to(fn, NTFMT_CHR_ZERO, lim);
				fn(buf);
				if (flags.minus) fill_chr_to(fn, NTFMT_CHR_SPACE, flags.width - wid);
			}
		}
	}
}

#endif
