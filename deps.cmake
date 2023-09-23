include(FetchContent)

set(BOOST_INCLUDE_LIBRARIES container pfr multi_index)
set(BOOST_ENABLE_CMAKE ON)
set(FETCHCONTENT_QUIET FALSE) # Needed to print downloading progress
FetchContent(
  Boost
  URL https://github.com/boostorg/boost/releases/download/boost-1.83.0/boost-1.83.0.tar.xz
)