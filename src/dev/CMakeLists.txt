cmake_minimum_required(VERSION 3.25)

add_library_checked_headers (
    dev INTERFACE
    dev_concepts.h
    dev_functional.h
    dev_stringify.h
    dev_tuple.h
    dev_type_list.h
    dev_type_traits.h
)
target_include_directories(dev INTERFACE .)