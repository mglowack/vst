#include <vst_defs_fields.h>
#include <vst_defs_with_fields.h>

#include <tuple>
#include <vector>
#include <variant>

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