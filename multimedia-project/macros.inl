#ifndef MACROS_INL
#define MACROS_INL

#ifndef STDAFX_H
#	error \
			"This is an extension for precompiled header stdafx.h, cannot be used alone"
#else
#	define MAKE_NOT_COPYABLE(T)  \
		T(const T& other) = delete; \
		T& operator=(const T& other) = delete;
#	define MAKE_NOT_MOVEABLE(T) \
		T(T&& other) = delete;     \
		T& operator=(T&& other) = delete;
#	define MAKE_NOT_COPYABLE_NOR_MOVEABLE(T) \
		MAKE_NOT_COPYABLE(T)                    \
		MAKE_NOT_MOVEABLE(T)
#endif

#endif  // !MACROS_INL
