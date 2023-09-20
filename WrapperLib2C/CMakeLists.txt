set(LIB_NAME lib_demo)
add_library(${LIB_NAME} SHARED
    src/lib-demo.cpp)
target_include_directories(${LIB_NAME} PUBLIC include)


add_executable(WrapperLib2C src/lib-cpp-main.cpp)
target_include_directories(WrapperLib2C PUBLIC include)
target_link_libraries(WrapperLib2C ${LIB_NAME})


add_executable(LibCMain src/lib-c-main.c src/lib-demo-wrapper.cpp)
target_include_directories(LibCMain PUBLIC include)
target_link_libraries(LibCMain ${LIB_NAME})