cmake_minimum_required(VERSION 3.23)

project(nbody LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
               Debug Release RelWithDebInfo MinSizeRel)
endif()

add_executable(nbody)

target_compile_features(nbody PRIVATE cxx_std_20)
set_target_properties(nbody PROPERTIES CXX_EXTENSIONS OFF)

target_sources(nbody
  PRIVATE
    src/main.cpp
  PRIVATE
    FILE_SET HEADERS
    BASE_DIRS src
    FILES
      src/api/concepts.hpp
      src/api/real.hpp
      src/datatype/bodies.hpp
      src/datatype/forces.hpp
      src/impl/time_integration.hpp
      src/impl/force_model.hpp
)

find_package(OpenMP REQUIRED)
target_link_libraries(nbody PRIVATE OpenMP::OpenMP_CXX)

# SYSTEM: suppress warnings coming from MIPP headers
target_include_directories(nbody SYSTEM PRIVATE external/MIPP/include)

# Warnings (GCC and Clang)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(nbody PRIVATE -Wall -Wextra -Wshadow)
endif()

option(NBODY_NATIVE "Optimize for the host CPU (-march=native)" ON)
if(NBODY_NATIVE)
  include(CheckCXXCompilerFlag)
  check_cxx_compiler_flag(-march=native HAS_MARCH_NATIVE)
  if(HAS_MARCH_NATIVE)
    target_compile_options(nbody PRIVATE -march=native)
  endif()
endif()

option(NBODY_VEC_REPORT "Print the vectorization report (GCC)" OFF)
if(NBODY_VEC_REPORT AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(nbody PRIVATE -fopt-info-vec-optimized)
endif()

option(NBODY_WARN_DOUBLE_PROMOTION "Warn about implicit float -> double conversions" OFF)
if(NBODY_WARN_DOUBLE_PROMOTION AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(nbody PRIVATE -Wdouble-promotion)
endif()

# find_package(Kokkos 4.7 REQUIRED CONFIG)
# target_link_libraries(nbody PRIVATE Kokkos::kokkos)
