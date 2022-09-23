#ifndef _debug_h
#define _debug_h


#if defined(DEBUG)

#include <stdio.h>

#define DEBUG_LOGLN(...) \
	fprintf(stdout, "%s:%d: ", __FILE__, __LINE__); \
	fprintf(stdout, __VA_ARGS__); \
	fprintf(stdout, "\n")

#else

#define DEBUG_LOGLN(...)

#endif

#endif /* _debug_h */
