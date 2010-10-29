#ifndef NTFMT_COMPAT_HPP_
#define NTFMT_COMPAT_HPP_

#include <stddef.h>
#include <boost/config.hpp>
#include <boost/cstdint.hpp>
#include <boost/integer.hpp>

#if defined(__GNUC__) && !defined(__clang__)
#include <stdint.h>
#define ntfmt_likely(x)       __builtin_expect((x),1)
#define ntfmt_unlikely(x)     __builtin_expect((x),0)
#else
#define ntfmt_likely(x)       (x)
#define ntfmt_unlikely(x)     (x)
#endif


namespace ntfmt {
	namespace detail {
		typedef ::size_t size_t;
#if defined(__GNUC__)
		typedef __UINTPTR_TYPE__ uintptr_t;
#elif defined(_MSC_VER)
		typedef ::uintptr_t uintptr_t;
#else
		typedef typename boost::uint_t<sizeof(void *)>::type uintptr_t;
#endif
	}
}

#endif
