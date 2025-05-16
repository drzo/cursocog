#ifndef _OPENCOG_MACROS_H
#define _OPENCOG_MACROS_H

// Windows compatibility utilities
#include "cpp_keywords.h"

#ifdef _MSC_VER
// From libpng
#pragma warning(disable:4996)

// Prevent windows.h from including min and max macros
#ifndef NOMINMAX
#define NOMINMAX
#endif

#endif // _MSC_VER

#define EVEN(n) (((n) % 2) == 0)
#define ODD(n) (((n) % 2) != 0)

#endif // _OPENCOG_MACROS_H 