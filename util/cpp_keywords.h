/*
 * cpp_keywords.h
 *
 * C++ keyword compatibility across different compilers
 *
 * Copyright (C) 2025 OpenCog Foundation
 * All Rights Reserved
 */

#ifndef _OPENCOG_CPP_KEYWORDS_H
#define _OPENCOG_CPP_KEYWORDS_H

// MSVC doesn't enable the "not" keyword by default
// This header provides compatibility
#ifdef _MSC_VER
#include <iso646.h>
// iso646.h provides:
// and = &&
// or = ||
// not = !
// not_eq = !=
// bitand = &
// bitor = |
// xor = ^
// compl = ~
// and_eq = &=
// or_eq = |=
// xor_eq = ^=
#endif

#endif // _OPENCOG_CPP_KEYWORDS_H 