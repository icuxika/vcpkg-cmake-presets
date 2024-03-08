if (UNIX AND APPLE)
    message(STATUS ">>> macOS")
    find_package(PkgConfig REQUIRED)
    set(ENV{PKG_CONFIG_PATH}
            "/Users/icuxika/CommandLineTools/ffmpeg6/lib/pkgconfig")

    pkg_check_modules(
            ffmpeg
            REQUIRED
            IMPORTED_TARGET
            libavcodec
            libavdevice
            libavfilter
            libavformat
            libavutil
            libpostproc
            libswresample
            libswscale)
elseif (UNIX)
    message(STATUS ">>> Linux")
else ()
    message(STATUS ">>> Windows")
endif ()

file(GLOB SOURCES "src/*.cpp")

set(LIB_NAME alpha-ffmpeg)
add_library(${LIB_NAME} SHARED ${SOURCES})
target_include_directories(${LIB_NAME} PUBLIC include)

if (UNIX AND APPLE)
    message(STATUS ">>> macOS")
    target_link_libraries(${LIB_NAME} PRIVATE PkgConfig::ffmpeg)
elseif (UNIX)
    message(STATUS ">>> Linux")
    target_include_directories(${LIB_NAME} PRIVATE /usr/include/x86_64-linux-gnu)
    target_link_directories(${LIB_NAME} PRIVATE /usr/lib/x86_64-linux-gnu)
    target_link_libraries(${LIB_NAME} PRIVATE avcodec avdevice avfilter avformat avutil postproc swresample swscale)
else ()
    message(STATUS ">>> Windows")
    target_include_directories(${LIB_NAME} PUBLIC libffmpeg\\include)
    target_link_directories(${LIB_NAME} PUBLIC libffmpeg\\lib\\x64)

    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        message(STATUS ">>> Windows MSVC")
        target_link_libraries(${LIB_NAME} PUBLIC libavcodec libavformat libavutil libswresample libswscale)
    endif ()
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # clang-cl 使用静态库有一些报错，暂时还不能解决，目前依旧使用导入库+动态库的方式
        message(STATUS ">>> Windows Clang")
        target_link_libraries(${LIB_NAME} PUBLIC avcodec avdevice avfilter avformat avutil postproc swresample swscale)
    endif ()
endif ()


file(GLOB HEADERS "include/*.h")
set_target_properties(${LIB_NAME} PROPERTIES PUBLIC_HEADER "${HEADERS}")

include(GNUInstallDirs)
install(TARGETS ${LIB_NAME}
        RUNTIME DESTINATION ${LIB_NAME}/${CMAKE_INSTALL_BINDIR}
        ARCHIVE DESTINATION ${LIB_NAME}/${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${LIB_NAME}/${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${LIB_NAME}/${CMAKE_INSTALL_INCLUDEDIR}
)

add_executable(FFmpegLibMain main.cpp)
target_include_directories(FFmpegLibMain PRIVATE "${PROJECT_SOURCE_DIR}/FFmpegLib")
target_link_libraries(FFmpegLibMain PRIVATE ${LIB_NAME})