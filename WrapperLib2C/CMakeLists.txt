set(LIB_CPP_NAME lib-cpp-demo)
add_library(${LIB_CPP_NAME} SHARED src/lib-demo.cpp)
target_include_directories(${LIB_CPP_NAME} PUBLIC include)

add_executable(LibCppMain src/lib-cpp-main.cpp)
target_link_libraries(LibCppMain ${LIB_CPP_NAME})


set(LIB_NAME lib-c-demo)
add_library(${LIB_NAME} SHARED src/lib-demo.cpp src/lib-demo-wrapper.cpp)
target_include_directories(${LIB_NAME} PUBLIC include)

add_executable(LibCMain src/lib-c-main.c)
target_link_libraries(LibCMain ${LIB_NAME})