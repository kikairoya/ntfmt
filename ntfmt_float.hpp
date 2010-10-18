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
		inline bool isinf(float d) { return !::_finite(d); }
		inline bool isinf(double d) { return !::_finite(d); }
		inline bool isinf(long double d) { return !::_finite((double)d); }
		inline bool isnan(float d) { return !!::_isnan(d); }
		inline bool isnan(double d) { return !!::_isnan(d); }
		inline bool isnan(long double d) { return !!::_isnan((double)d); }
		inline float nextafter(float x, float y) { return (float)::_nextafter(x, y); }
		inline double nextafter(double x, double y) { return ::_nextafter(x, y); }
		inline long double nextafter(long double x, long double y) { return ::_nextafter((double)x, (double)y); }
#else
		using std::fabs;
		using std::floor;
		using std::isinf;
		using std::isnan;
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

		template <typename charT>
		inline void inc_strnum(charT *const strnum, unsigned const base, unsigned const col) {
			if (strnum[col]++ == to_hexstr<charT>(base-1)) {
				strnum[col] = to_hexstr<charT>(0);
				inc_strnum(strnum, base, col - 1);
			}
		}

		template <typename T>
		T ipow(unsigned const base, int e) {
			T v = 1;
			if (e < 0) {
				while (e++) v /= base;
			} else {
				while (e--) v *= base;
			}
			return v;
		}

		template <typename T>
		int ilog(unsigned const base, T v) {
			if (base <= v) {
				int r = 1;
				while (base <= v) {
					v /= base;
					++r;
				}
				return r;
			}
			int r = 2;
			while (base > v) {
				v *= base;
				--r;
			}
			return r;
		}

		template <typename T, typename U>
		struct select_larger_type: if_c<(sizeof(T)<sizeof(U)), U, T> { };
		template <typename T>
		struct dtoa_traits {
			typedef typename boost::int_max_value_t<numeric_limits<T>::max_exponent>::least higher_type;
			typedef typename boost::int_min_value_t<numeric_limits<T>::min_exponent>::least lower_type;
			typedef typename boost::integral_promotion<typename select_larger_type<higher_type, lower_type>::type>::type return_type;
		};
		template <typename charT, typename T>
		typename dtoa_traits<T>::return_type dtoa(charT *const out, T v, int prec, unsigned const base) {
			if (prec < 0) prec = 0;
			if (prec > numeric_limits<T>::digits10) prec = numeric_limits<T>::digits10;
			typedef typename dtoa_traits<T>::return_type return_type;
			v = fabs(v);
			return_type const k = ilog(base, get_successor(v));
			
			charT buf[numeric_limits<T>::digits + 3] = { };
			buf[0] = NTFMT_CHR_ZERO;
			if (prec >= numeric_limits<T>::digits-3) prec = numeric_limits<T>::digits-3;
			{
				T q = v / ipow<T>(base, k);
				charT const *const end = array_end(buf);
				for (charT *ptr = array_begin(buf) + 1; ptr < end; ++ptr) {
					q *= base;
					T const fq = floor(q);
					*ptr = to_hexstr<charT>(static_cast<unsigned>(fq)%base);
					q -= fq;
				}
			}
			if (static_cast<unsigned>(from_hexstr(buf[prec+1])) >= base/2) inc_strnum(buf, base, prec);
			
			if (ntfmt_unlikely(buf[0] != NTFMT_CHR_ZERO)) {
				gstrncpy(out, buf, prec+1);
				out[prec] = 0;
				return k - 1;
			}
			gstrncpy(out, buf+1, prec+1);
			out[prec] = 0;
			return k;
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
			if (value < 0) *phead++ = NTFMT_CHR_MINUS;
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
			charT buf[numeric_limits<T>::digits10 + 8];
			if (exponential) {
				int const e = dtoa<charT, T>(buf, value, prec + flags.exponential, flags.radix);
				if (!flags.exponential && !flags.alter) {
					charT *p = buf + gstrlen(buf) - 1;
					while (*p == NTFMT_CHR_ZERO) *p-- = 0; 
				}
				*phead++ = buf[0];
				buf[0] = buf[1] && (prec!=0 || flags.alter) ? NTFMT_CHR_DOT : 0;

				size_t const wid = gstrlen(head) + gstrlen(buf) + 5;
				if (!flags.minus) for (size_t n = wid; n < flags.width; ++n) fn(NTFMT_CHR_SPACE);
				fn(head);
				fn(buf);
				fn(flags.capital ? NTFMT_CH_LIT('E') : NTFMT_CH_LIT('e'));
				flags_t ff = default_flags();
				ff.zero = 1;
				ff.prec_enable = 1;
				ff.precision = 3;
				ff.plus = 1;
				integer_printer(fn, e-1, ff);
				if (flags.minus) for (size_t n = wid; n < flags.width; ++n) fn(NTFMT_CHR_SPACE);
			} else {
				int const e = dtoa<charT, T>(buf, value, prec + (flags.fixed ? l : 0), flags.radix);
				if (e < 1) {
					*phead++ = NTFMT_CHR_ZERO;
					*phead++ = NTFMT_CHR_DOT;
					if (e <= -prec) buf[0] = 0;
					int const lim = e < -prec ? prec : -e;
					size_t const wid = gstrlen(head) + gstrlen(buf) + lim;
					if (!flags.minus) for (size_t n = wid; n < flags.width; ++n) fn(NTFMT_CHR_SPACE);
					fn(head);
					for (int n = 0; n < lim; ++n) fn(NTFMT_CHR_ZERO);
					fn(buf);
					if (flags.minus) for (size_t n = wid; n < flags.width; ++n) fn(NTFMT_CHR_SPACE);
				} else {
					std::copy_backward(array_begin(buf)+e, array_begin(buf)+gstrlen(buf), array_begin(buf)+e+1);
					buf[e] = NTFMT_CHR_DOT;
					if (!flags.fixed && !flags.alter) {
						charT *p = buf + gstrlen(buf) - 1;
						while (*p == NTFMT_CHR_ZERO) *p-- = 0;
						if (*p == NTFMT_CHR_DOT) *p-- = 0;
					}
					size_t const wid = gstrlen(head) + gstrlen(buf);
					if (!flags.minus) for (size_t n = wid; n < flags.width; ++n) fn(NTFMT_CHR_SPACE);
					fn(head);
					fn(buf);
					if (flags.minus) for (size_t n = wid; n < flags.width; ++n) fn(NTFMT_CHR_SPACE);
				}
			}
		}
	}
}

#endif
