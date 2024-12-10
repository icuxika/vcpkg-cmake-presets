if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    set_source_files_properties(
            ${PROJECT_SOURCE_DIR}/SkiaUI/resource/WindowsProject.rc
            PROPERTIES
            VS_TOOL_OVERRIDE "RC"
    )

    add_executable(SkiaUI WIN32 src/WindowsProject.cpp)
    target_include_directories(SkiaUI PRIVATE include)
    target_include_directories(SkiaUI PRIVATE skia)
    target_link_directories(SkiaUI PRIVATE skia\\lib)
    target_link_libraries(SkiaUI PRIVATE skia)
    target_sources(
            SkiaUI
            PRIVATE
            ${PROJECT_SOURCE_DIR}/SkiaUI/resource/WindowsProject.rc
    )
endif ()