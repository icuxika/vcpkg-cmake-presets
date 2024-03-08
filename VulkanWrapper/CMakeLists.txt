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

    file(GLOB SOURCES "src/*.cpp")
    add_executable(
            VulkanWrapper
            main.cpp
            ${SOURCES})

    target_link_libraries(VulkanWrapper PRIVATE Vulkan::Vulkan glfw glm
            cxxopts::cxxopts imgui::imgui)
    target_include_directories(VulkanWrapper PRIVATE include)
    target_include_directories(VulkanWrapper PRIVATE ${Stb_INCLUDE_DIR})
    target_include_directories(VulkanWrapper PRIVATE "${PROJECT_SOURCE_DIR}/FFmpegLib")
    target_link_libraries(VulkanWrapper PRIVATE alpha-ffmpeg)

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

    add_executable(alphaFFmpegDemo alpha-ffmpeg-demo.cpp)
    target_include_directories(alphaFFmpegDemo PRIVATE "${PROJECT_SOURCE_DIR}/FFmpegLib")
    target_link_libraries(alphaFFmpegDemo PRIVATE alpha-ffmpeg)
endif (${Vulkan_FOUND})
