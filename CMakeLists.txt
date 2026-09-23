cmake_minimum_required(VERSION 3.23)

project(nbody LANGUAGES CXX)

add_executable(nbody)

# Standard C++ (ajuste selon ce que tu utilises : 17, 20, 23)
target_compile_features(nbody PRIVATE cxx_std_20)

target_sources(nbody
  PRIVATE
    src/main.cpp
  PRIVATE
    FILE_SET HEADERS
    BASE_DIRS src
    FILES
      src/api/concepts.hpp
      src/datatype/bodies.hpp
      src/impl/time_integration.hpp
      src/impl/force_model.hpp
)

find_package(OpenMP REQUIRED)
target_link_libraries(nbody PRIVATE OpenMP::OpenMP_CXX)

target_include_directories(nbody PRIVATE external/MIPP/include)

# find_package(Kokkos 4.7 REQUIRED CONFIG)
# target_link_libraries(nbody PRIVATE Kokkos::kokkos)
