cmake_minimum_required(VERSION 3.25)

add_library_checked_headers (
    vst INTERFACE vst.hpp

    vst_defs_fields.h
    vst_defs_ops.h
    vst_defs_with_fields.h
    vst_indexed_var.h
    vst_make_type.h
    vst_named_field_ptr.h
    vst_named_var.h
    vst_operators_addable.h
    vst_operators_comparable.h
    vst_operators_hashable.h
    vst_operators_ordered.h
    vst_operators_stream.h
    vst_trait.h
    vst_wrapped_value.h
)
target_include_directories(vst INTERFACE .)
target_link_libraries(vst INTERFACE dev Boost::container Boost::pfr)
