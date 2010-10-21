#ifndef NTFMT_COMPAT_HPP_
#define NTFMT_COMPAT_HPP_

#include <stddef.h>
#include <boost/config.hpp>

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
#if defined(__clang__) && defined(__linux__)
		typedef unsigned __INTPTR_TYPE__ uintptr_t;
#else
		typedef ::uintptr_t uintptr_t;
#endif
	}
}

#endif
