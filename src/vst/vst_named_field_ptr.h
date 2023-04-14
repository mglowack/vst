#pragma once

namespace vst {

// ###################
// # named_field_ptr #
// ###################

template<typename field_ptr_t>
struct named_field_ptr
{
    const char* name;
    field_ptr_t field_ptr;

    constexpr explicit named_field_ptr(const char* name, field_ptr_t field_ptr)
    : name(name), field_ptr(field_ptr) {}
};

#define MEMBER(obj, x) ::vst::named_field_ptr{#x, &obj::x}

} // namespace vst