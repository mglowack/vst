#pragma once

namespace dev {

template<typename = void>
struct plus_assign
{
    template<typename T>
    [[maybe_unused]] T& operator()(T& lhs, const T& rhs)
    {
        return lhs += rhs;
    }
};

template<typename = void>
struct minus_assign
{
    template<typename T>
    [[maybe_unused]] T& operator()(T& lhs, const T& rhs)
    {
        return lhs -= rhs;
    }
};

} // namespace dev