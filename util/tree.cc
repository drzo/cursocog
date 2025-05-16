#ifdef WIN32
// For Windows, disable the parsing functionality in tree.cc
#include <string>
#include <iostream>
#include "tree.h"

namespace opencog {

std::istream& operator>>(std::istream& in, tree&) {
    // Not implemented for Windows
    return in;
}

} // namespace opencog

#else
// Original implementation for Unix/Linux
// boost/spirit/include/classic_core.hpp needs to be installed on Unix
#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_file_iterator.hpp>
#include <iostream>

#include "platform.h"
#include "tree.h"

// The rest of the original implementation goes here...
#endif 