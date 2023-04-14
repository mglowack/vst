#pragma once

#include <vst_named_field_ptr.h>

#include <dev_type_list.h>
#include <dev_type_traits.h>

#include <tuple>
#include <type_traits>

// ###########################
// # is_pointer_to_member_of #
// ###########################

template<typename T>
struct is_pointer_to_member_of
{
    template<typename ptr_t>
    struct pred : std::false_type {};

    template<typename X>
    struct pred<X (T::*)> : std::true_type {};
};

// #########################
// # is_named_field_ptr_of #
// #########################

template<typename T>
struct is_named_field_ptr_of
{
    template<typename ptr_t>
    struct pred : std::false_type {};

    template<typename X>
    struct pred<vst::named_field_ptr<X (T::*)>> : std::true_type {};
};

// ###############
// # FieldSpecOf #
// ###############

template<typename spec_t, typename T>
concept FieldSpecOf =
    dev::is_template_v<std::tuple, spec_t>
    && (dev::type_list_all_v<dev::template_cast_t<dev::type_list, spec_t>, is_pointer_to_member_of<T>::template pred>
      || dev::type_list_all_v<dev::template_cast_t<dev::type_list, spec_t>, is_named_field_ptr_of<T>::template pred>);


// ################
// # DescribesThe #
// ################

template<typename T, typename U>
concept DescribesThe = requires {
    { T::get_fields() } -> FieldSpecOf<U>;
};

// #################
// # SelfDescribed #
// #################

template<typename T>
concept SelfDescribed = DescribesThe<T, T>;