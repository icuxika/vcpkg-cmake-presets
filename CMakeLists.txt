cmake_minimum_required(VERSION 3.15)
project(VcpkgCmakePresets)
set(CMAKE_CXX_STANDARD 20)

find_package(fmt REQUIRED)
find_package(range-v3 REQUIRED)
find_package(cxxopts REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    fmt::fmt-header-only
    range-v3::range-v3
    cxxopts::cxxopts)

install(TARGETS ${PROJECT_NAME}
        DESTINATION Release/bin)

add_subdirectory(Algorithms)
add_subdirectory(FFmpegLib)
add_subdirectory(SingleFilePrograms)
add_subdirectory(VulkanWrapper)
add_subdirectory(WebUI)
add_subdirectory(WrapperLib2C)