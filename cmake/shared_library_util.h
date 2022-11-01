#ifndef @UPPERCASE_TARGETNAME@_SHARED_LIBRARY_UTIL_H_INCLUDED_
#define @UPPERCASE_TARGETNAME@_SHARED_LIBRARY_UTIL_H_INCLUDED_

/* This header file is meant to be compatible to both C and C++ programs. */

/* When building or linking the library as static library, define @UPPERCASE_TARGETNAME@_STATIC */
#ifndef @UPPERCASE_TARGETNAME@_STATIC
	/*
	 * Platform dependent dynamic linking:
	 * Handles export/hiding of symbols.
	 */
	#if defined(_WIN32) || defined(__WIN32__)
		#ifdef @UPPERCASE_TARGETNAME@_BUILD_SHARED
			#define @UPPERCASE_TARGETNAME@_API_PUBLIC __declspec(dllexport)
			#define @UPPERCASE_TARGETNAME@_API_PRIVATE
		#else
			#define @UPPERCASE_TARGETNAME@_API_PUBLIC __declspec(dllimport)
			#define @UPPERCASE_TARGETNAME@_API_PRIVATE
		#endif
	#else
		#if __GNUC__ >= 4
			#define @UPPERCASE_TARGETNAME@_API_PUBLIC  __attribute__((__visibility__ ("default")))
			#define @UPPERCASE_TARGETNAME@_API_PRIVATE __attribute__((__visibility__ ("hidden")))
		#else
			#define @UPPERCASE_TARGETNAME@_API_PUBLIC
			#define @UPPERCASE_TARGETNAME@_API_PRIVATE
		#endif
	#endif
#else
	#define @UPPERCASE_TARGETNAME@_API_PUBLIC
	#define @UPPERCASE_TARGETNAME@_API_PRIVATE
#endif

#endif
