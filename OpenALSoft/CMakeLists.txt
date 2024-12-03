find_package(OpenAL CONFIG REQUIRED)

add_executable(OpenALSoft main.cpp)
target_link_libraries(OpenALSoft PRIVATE OpenAL::OpenAL alpha-ffmpeg-static)

