cmake_minimum_required(VERSION 3.23)

project(nbody)

add_executable(nbody)
target_sources(nbody
  PRIVATE
    src/main.cpp)

find_package(Kokkos 4.7 REQUIRED CONFIG)
target_link_libraries(nbody PRIVATE Kokkos::kokkos)
