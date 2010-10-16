#ifndef NTFMT_FLOAT_HPP_
#define NTFMT_FLOAT_HPP_

#include "ntfmt_fwd.hpp"

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
		template <typename T>
		void integer_printer(sink_fn_t &fn, T const &value, flags_t const &flags);

		template <typename T>
		inline T get_predecessor(T const &v) { return nextafter(v, -numeric_limits<T>::infinity()); }
		template <typename T>
		inline T get_successor(T const &v) { return nextafter(v, +numeric_limits<T>::infinity()); }

		inline void inc_strnum(char *const strnum, unsigned const base, unsigned const col) {
			if (strnum[col]++ == hexstr[base-1]) {
				strnum[col] = hexstr[0];
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
		template <typename T>
		typename dtoa_traits<T>::return_type dtoa(char *const out, T v, int prec, unsigned const base) {
			if (prec < 0) prec = 0;
			if (prec > numeric_limits<T>::digits10) prec = numeric_limits<T>::digits10;
			typedef typename dtoa_traits<T>::return_type return_type;
			v = fabs(v);
			return_type const k = ilog(base, get_successor(v));
			
			char buf[numeric_limits<T>::digits + 3] = "0";
			if (prec >= numeric_limits<T>::digits-3) prec = numeric_limits<T>::digits-3;
			{
				T q = v / ipow<double>(base, k);
				for (size_t i=1; i<sizeof(buf)-1; ++i) {
					q *= base;
					T const fq = floor(q);
					buf[i] = hexstr[static_cast<unsigned>(fq)%base];
					q -= fq;
				}
			}
			if ((unsigned)(buf[prec+1]-'0') >= base/2) inc_strnum(buf, base, prec);
			
			if (ntfmt_unlikely(buf[0] != '0')) {
				strncpy(out, buf, prec+1);
				out[prec] = 0;
				return k - 1;
			}
			strncpy(out, buf+1, prec+1);
			out[prec] = 0;
			return k;
		}
		template <typename T>
		void float_printer(sink_fn_t &fn, T const &value, const flags_t &flags) {
			using namespace std;
			if (ntfmt_unlikely(isnan(value))) {
				fn(flags.capital ? "NAN" : "nan");
				return;
			}
			char head[6] = { };
			char *phead = head;
			if (value < 0) *phead++ = '-';
			else if (flags.plus) *phead++ = '+';
			else if (flags.space) *phead++ = ' ';
			if (ntfmt_unlikely(isinf(value))) {
				fn(head);
				fn(flags.capital ? "INF" : "inf");
				return;
			}
			int const prec = flags.prec_enable ? flags.precision : 6;
			int const l = ilog(flags.radix, fabs(value));
			bool const exponential = flags.exponential || ( !flags.fixed && ((l < -4) || (l > prec)));
			char buf[numeric_limits<T>::digits10 + 8];
			if (exponential) {
				int const e = dtoa(buf, value, prec + flags.exponential, flags.radix);
				if (!flags.exponential && !flags.alter) {
					char *p = buf + strlen(buf) - 1;
					while (*p == '0') *p-- = 0; 
				}
				*phead++ = buf[0];
				buf[0] = buf[1] && (prec!=0 || flags.alter) ? '.' : 0;

				size_t const wid = strlen(head) + strlen(buf) + 5;
				if (!flags.minus) for (size_t n = wid; n < flags.width; ++n) fn(' ');
				fn(head);
				fn(buf);
				fn(flags.capital ? 'E' : 'e');
				flags_t ff = default_flags();
				ff.zero = 1;
				ff.prec_enable = 1;
				ff.precision = 3;
				ff.plus = 1;
				integer_printer(fn, e-1, ff);
				if (flags.minus) for (size_t n = wid; n < flags.width; ++n) fn(' ');
			} else {
				int const e = dtoa(buf, value, prec + (flags.fixed ? l : 0), flags.radix);
				if (e < 1) {
					*phead++ = '0';
					*phead++ = '.';
					if (e <= -prec) buf[0] = 0;
					int const lim = e < -prec ? prec : -e;
					size_t const wid = strlen(head) + strlen(buf) + lim;
					if (!flags.minus) for (size_t n = wid; n < flags.width; ++n) fn(' ');
					fn(head);
					for (int n = 0; n < lim; ++n) fn('0');
					fn(buf);
					if (flags.minus) for (size_t n = wid; n < flags.width; ++n) fn(' ');
				} else {
					memmove(buf+e+1, buf+e, sizeof(buf)-e-1);
					buf[e] = '.';
					if (!flags.fixed && !flags.alter) {
						char *p = buf + strlen(buf) - 1;
						while (*p == '0') *p-- = 0;
						if (*p == '.') *p-- = 0;
					}
					size_t const wid = strlen(head) + strlen(buf);
					if (!flags.minus) for (size_t n = wid; n < flags.width; ++n) fn(' ');
					fn(head);
					fn(buf);
					if (flags.minus) for (size_t n = wid; n < flags.width; ++n) fn(' ');
				}
			}
		}
	}
}

#endif
