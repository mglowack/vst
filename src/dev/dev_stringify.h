#pragma once

#include <utility>
#include <iosfwd>
#include <sstream>

namespace dev {

// #############
// # stringify #
// #############

template<typename T>
std::string stringify(const T& o)
{
    std::ostringstream oss;
    oss << o;
    return oss.str();
}

} // namespace dev