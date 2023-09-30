cmake_minimum_required(VERSION 3.25)

add_library_checked_headers (
    named_type INTERFACE named_type.hpp

    named_type_conversions_category.h
    named_type_ops_category.h
    named_type_pod.h
    named_type_trait.h
)
target_include_directories(named_type INTERFACE . vst)
target_link_libraries(named_type INTERFACE vst dev)