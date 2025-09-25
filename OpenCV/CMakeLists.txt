find_package(OpenCV CONFIG REQUIRED)
add_executable(OpenCVMain main.cpp)
target_link_libraries(OpenCVMain ${OpenCV_LIBS})