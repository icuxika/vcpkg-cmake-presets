cmake_minimum_required(VERSION 3.15)
project(VcpkgCmakePresets)
set(CMAKE_CXX_STANDARD 20)

find_package(fmt REQUIRED)
find_package(range-v3 REQUIRED)
find_package(cxxopts REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    fmt::fmt
    range-v3::range-v3
    cxxopts::cxxopts)