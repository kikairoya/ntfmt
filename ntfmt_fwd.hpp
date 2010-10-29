#ifndef NTFMT_FWD_HPP_
#define NTFMT_FWD_HPP_

#include "ntfmt_compat.hpp"

namespace ntfmt {
	struct packed_flags_t {
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
	struct unpacked_flags_t {
		short precision;
		short radix;
		short width;
		bool minus;
		bool space;
		bool alter;
		bool zero;
		bool plus;
		bool capital;
		bool character;
		bool prec_enable;
		bool width_enable;
		bool exponential;
		bool fixed;
		operator packed_flags_t() const {
			packed_flags_t f = { minus, space, alter, zero, plus, capital, character, prec_enable, precision, radix, width_enable, exponential, fixed, width };
			return f;
		}
	};
#ifdef NTFMT_USE_PACKED_FLAGS
	typedef packed_flags_t flags_t;
#else
	typedef unpacked_flags_t flags_t;
#endif

	template <typename T>
	struct fmt_t;

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

		struct sink_fn_base: noncopyable {
			virtual int operator ()(char const *s)  { int n; for (n = 0; *s && (*this)(*s++) >= 0; ++n) ; return n; }
			virtual int operator ()(char) { return -1; }
			virtual int operator ()(wchar_t const *s)  { int n; for (n = 0; *s && (*this)(*s++) >= 0; ++n) ; return n; }
			virtual int operator ()(wchar_t) { return -1; }
			~sink_fn_base() { }
		};
	}

	template <typename charT>
	struct sink_fn_t: detail::sink_fn_base {
		typedef charT char_type;
		int operator ()(charT c) = 0;
		using sink_fn_base::operator ();
		template <typename T>
		sink_fn_t &operator <<(T const &v) { fmt(v).print(*this); return *this; }
		template <typename T>
		sink_fn_t &operator <<(fmt_t<T> const &v) { v.print(*this); return *this; }
		template <typename T>
		sink_fn_t const &operator <<(T const &v) const { fmt(v).print(*this); return *this; }
		template <typename T>
		sink_fn_t const &operator <<(fmt_t<T> const &v) const { v.print(*this); return *this; }
#ifdef BOOST_HAS_VARIADIC_TMPL
		sink_fn_t &format() { return *this; }
		sink_fn_t const &format() const { return *this; }
		template <typename A1, typename... Args>
		sink_fn_t &format(A1 const &a1, Args const &...args) { fmt(a1).print(*this); return format(args...); }
		template <typename A1, typename... Args>
		sink_fn_t &format(fmt_t<A1> const &a1, Args const &...args) { a1.print(*this); return format(args...); }
		template <typename A1, typename... Args>
		sink_fn_t const &format(fmt_t<A1> const &a1, Args const &...args) const { return const_cast<sink_fn_t *>(this)->format(a1).format(std::forward<Args>(args)...); }
		template <typename A1, typename... Args>
		sink_fn_t const &format(A1 const &a1, Args const &...args) const { return const_cast<sink_fn_t *>(this)->format(a1).format(std::forward<Args>(args)...); }
#endif
	};

}

#endif
