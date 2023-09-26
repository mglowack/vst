FROM ubuntu:23.04

RUN apt-get -y update && apt-get install -y \
  g++-13 clang-15 \
  gdb clang-tidy clang-tools \
  libc++-dev libc++abi-dev \
  cmake ninja-build \
  git \
  vim 

