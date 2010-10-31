#ifndef NTFMT_COMPAT_HPP_
#define NTFMT_COMPAT_HPP_

#include <stddef.h>
#include <boost/config.hpp>
#include <boost/cstdint.hpp>

#if defined(__GNUC__) && !defined(__clang__)
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
		typedef __typeof__(((int *)0-(int *)0)) uintptr_t;
#elif defined(_MSC_VER)
		typedef ::uintptr_t uintptr_t;
#else
		typedef size_t uintptr_t;
#endif
	}
}

#endif
