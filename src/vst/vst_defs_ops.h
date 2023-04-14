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

namespace vst::op { struct ordered;  }
namespace vst::op { struct hashable; }
namespace vst::op { struct addable;  }
namespace vst {

// ############
// # Operator #
// ############

template<typename T>
struct is_op : dev::type_list_contains<dev::type_list<op::ordered, op::hashable, op::addable>, T> {};

template<typename T>
constexpr bool is_op_v = is_op<T>::value;

template<typename T>
concept Operator = is_op_v<T>;

static_assert(!is_op_v<int>);
static_assert(!is_op_v<float>);
static_assert( is_op_v<op::ordered>);
static_assert( is_op_v<op::hashable>);
static_assert( is_op_v<op::addable>);

} // namespace vst

namespace vst::with_fields {

template<auto (*get_fields_func)()>
struct from_func
{
    static constexpr auto get_fields()
    {
        return get_fields_func();
    }
};

template<auto fields>
struct from_var
{
    static constexpr auto get_fields()
    {
        return *fields;
    }
};

template<typename T>
struct from
{
    static constexpr auto get_fields()
    {
        static_assert(SelfDescribed<T>, "T must be an aggregate or have 'get_fields' defined.");
        return T::get_fields();
    }
};

struct empty
{
    static constexpr auto get_fields()
    {
        return std::tuple{};
    }
};

struct use_default {};

struct from_aggregate {};

} // namespace vst::with_fields

#include <vector>
#include <variant>

namespace Described_tests {

struct empty {};
static_assert(!SelfDescribed<empty>);

struct simple_empty_fields {
    static auto get_fields() {
        return std::tuple{};
    }
};
static_assert( SelfDescribed<simple_empty_fields>);
static_assert( DescribesThe<vst::with_fields::empty, simple_empty_fields>);

struct simple_with_fields {
    int i;
    float f;
    static auto get_fields() {
        return std::tuple{&simple_with_fields::i, &simple_with_fields::f};
    }
};
static_assert( SelfDescribed<simple_with_fields>);
static_assert( DescribesThe<vst::with_fields::from<simple_empty_fields>, simple_empty_fields>);

struct simple {
    int i;
    float f;
};
struct other {
    int i;
    float f;
};

static_assert(!FieldSpecOf<std::pair<int, float>,                                                                              simple>);
static_assert(!FieldSpecOf<std::vector<int>,                                                                                   simple>);
static_assert(!FieldSpecOf<std::variant<int, float>,                                                                           simple>);
static_assert( FieldSpecOf<std::tuple<>,                                                                                       simple>);
static_assert( FieldSpecOf<std::tuple<decltype(&simple::i)>,                                                                   simple>);
static_assert(!FieldSpecOf<std::tuple<decltype(&other::i)>,                                                                    simple>);
static_assert(!FieldSpecOf<std::tuple<decltype(&simple::i), decltype(&other::f)>,                                              simple>);
static_assert( FieldSpecOf<std::tuple<decltype(&simple::i), decltype(&simple::f)>,                                             simple>);
static_assert( FieldSpecOf<std::tuple<vst::named_field_ptr<decltype(&simple::i)>, vst::named_field_ptr<decltype(&simple::f)>>, simple>);
static_assert(!FieldSpecOf<std::tuple<vst::named_field_ptr<decltype(&simple::i)>, decltype(&simple::f)>,                       simple>);
static_assert(!FieldSpecOf<std::tuple<decltype(&simple::i), vst::named_field_ptr<decltype(&simple::f)>>,                       simple>);

}