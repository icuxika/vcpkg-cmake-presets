find_package(Vulkan REQUIRED)
if (${Vulkan_FOUND})
    message(STATUS "Found Vulkan")
    message(STATUS "Vulkan version: ${Vulkan_VERSION}")
    find_package(glfw3 REQUIRED)
    find_package(glm REQUIRED)
    find_package(Stb REQUIRED)
    find_package(cxxopts REQUIRED)
    find_package(imgui CONFIG REQUIRED)
    find_package(VulkanMemoryAllocator CONFIG REQUIRED)

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
    add_executable(
            VulkanWrapper
            main.cpp
            ${SOURCES})

    target_link_libraries(VulkanWrapper PRIVATE Vulkan::Vulkan glfw glm
            cxxopts::cxxopts imgui::imgui)
    target_include_directories(VulkanWrapper PRIVATE include)
    target_include_directories(VulkanWrapper PRIVATE ${Stb_INCLUDE_DIR})

    if (UNIX AND APPLE)
        message(STATUS ">>> macOS")
        target_link_libraries(VulkanWrapper PRIVATE PkgConfig::ffmpeg)
    elseif (UNIX)
        message(STATUS ">>> Linux")
        target_include_directories(VulkanWrapper PRIVATE /usr/include/x86_64-linux-gnu)
        target_link_directories(VulkanWrapper PRIVATE /usr/lib/x86_64-linux-gnu)
        target_link_libraries(VulkanWrapper PRIVATE avcodec avdevice avfilter avformat avutil postproc swresample swscale)
    else ()
        message(STATUS ">>> Windows")
        target_include_directories(VulkanWrapper PRIVATE libffmpeg\\include)
        target_link_directories(VulkanWrapper PRIVATE libffmpeg\\lib\\x64)
        if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
            message(STATUS ">>> Windows MSVC")
            target_link_libraries(VulkanWrapper PRIVATE libavcodec libavformat libavutil libswresample libswscale)
        endif ()
        if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            # clang-cl 使用静态库有一些报错，暂时还不能解决，目前依旧使用导入库+动态库的方式
            message(STATUS ">>> Windows Clang")
            target_link_libraries(VulkanWrapper PRIVATE avcodec avdevice avfilter avformat avutil postproc swresample swscale)
        endif ()
    endif ()

    add_custom_command(TARGET VulkanWrapper PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/VulkanWrapper/shaders-bin/ $<TARGET_FILE_DIR:VulkanWrapper>/shaders)

    # Vulkan Tutorial
    add_definitions(-DSOURCE_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}\")
    add_executable(VulkanDemo vulkan-tutorial-demo.cpp)
    target_include_directories(VulkanDemo PRIVATE include)
    target_link_libraries(VulkanDemo PRIVATE Vulkan::Vulkan glfw glm imgui::imgui)
    target_include_directories(VulkanDemo PRIVATE ${Stb_INCLUDE_DIR})
    add_custom_command(TARGET VulkanDemo PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/VulkanWrapper/demo-shaders-bin/ $<TARGET_FILE_DIR:VulkanDemo>/demo-shaders)
    add_custom_command(TARGET VulkanDemo PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/VulkanWrapper/textures/ $<TARGET_FILE_DIR:VulkanDemo>/textures)

    add_executable(VulkanTextureImGuiDemo vulkan-tutorial-texture-imgui-demo.cpp src/ui-components.cpp)
    target_include_directories(VulkanTextureImGuiDemo PRIVATE include)
    target_link_libraries(VulkanTextureImGuiDemo PRIVATE Vulkan::Vulkan glfw glm imgui::imgui GPUOpen::VulkanMemoryAllocator)
    # target_include_directories(VulkanTextureImGuiDemo PRIVATE ../ThirdPartyLibs/ImGUIExplore)

    add_executable(VulkanTriangleImGuiDemo vulkan-tutorial-triangle-imgui-demo.cpp)
    target_link_libraries(VulkanTriangleImGuiDemo PRIVATE Vulkan::Vulkan glfw glm imgui::imgui GPUOpen::VulkanMemoryAllocator)
    # target_include_directories(VulkanTriangleImGuiDemo PRIVATE ../ThirdPartyLibs/ImGUIExplore)
endif (${Vulkan_FOUND})
