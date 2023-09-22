add_executable(singleton singleton.cpp)
add_executable(uniquePtr unique-ptr.cpp)
add_executable(utf8Parse utf8-parse.cpp)
target_link_libraries(utf8Parse
    fmt::fmt-header-only
    range-v3::range-v3
    cxxopts::cxxopts)