#include <vst_defs_with_fields.h>

#include <tuple>

using namespace vst;

struct simple_pod {
    int x;
    float y;

// TODO: fix
    static constexpr auto get_fields()
    {
        return std::tuple{};
    }
};

template class with_fields::from<simple_pod>;

// constexpr auto get_simple_pod_fields() {
//     return std::tuple{
//         MEMBER(simple_pod, x),
//         MEMBER(simple_pod, y)};
// }

// constexpr auto k_simple_pod_fields = get_simple_pod_fields();

// template<typename... ops>
// using simple_explicit_default = vst::type<simple_self_described_pod, vst::with_fields::use_default, ops...>;

// template<typename... ops>
// using custom_from_func      = vst::type<simple_pod,
//                                         vst::with_fields::from_func<get_simple_pod_fields>,
//                                         ops...>;

// template<typename... ops>
// using custom_from_var       = vst::type<simple_pod,
//                                         vst::with_fields::from_var<&k_simple_pod_fields>,
//                                         ops...>;