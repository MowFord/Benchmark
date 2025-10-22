cmake_minimum_required(VERSION 3.24)
project(LuaBenchmarkExample)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# CPM.cmake
include(FetchContent)
FetchContent_Declare(
    cpm
    GIT_REPOSITORY https://github.com/cpm-cmake/CPM.cmake.git
    GIT_TAG origin/master
)
FetchContent_MakeAvailable(cpm)

# Externally provided libraries
FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.10.x)

FetchContent_Declare(googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG main) # need main for benchmark::benchmark

FetchContent_MakeAvailable(
        googletest
        googlebenchmark)

# Sol2
CPMAddPackage(
    NAME sol2
    GITHUB_REPOSITORY ThePhD/sol2
    VERSION 3.3.0
)
set(LUAJIT_INCLUDE_DIR "C:/VSCode/Benchmark/ext/luajit/include")
set(LUAJIT_LIBRARY "C:/VSCode/Benchmark/ext/luajit/lib64/libluajit_64.lib")


add_executable(benchmark_lua main.cpp)

target_link_libraries(benchmark_lua
    PRIVATE sol2::sol2
            ${LUAJIT_LIBRARY}
            benchmark::benchmark
)
target_include_directories(benchmark_lua PRIVATE
    ${LUAJIT_INCLUDE_DIR}
)
set_target_properties(benchmark_lua PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}"
  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}"
  RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_SOURCE_DIR}"
  RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_SOURCE_DIR}"
)