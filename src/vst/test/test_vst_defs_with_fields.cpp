#include <vst_defs_ops.h>

using namespace vst;

static_assert(!is_op_v<int>);
static_assert(!is_op_v<float>);
static_assert( is_op_v<op::ordered>);
static_assert( is_op_v<op::hashable>);
static_assert( is_op_v<op::addable>);