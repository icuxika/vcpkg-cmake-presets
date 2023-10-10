find_package(Vulkan REQUIRED)
if(${Vulkan_FOUND})
  message(STATUS "Found Vulkan")
  message(STATUS "Vulkan version: ${Vulkan_VERSION}")
  find_package(glfw3 REQUIRED)
  find_package(glm REQUIRED)
  find_package(Stb REQUIRED)
  find_package(cxxopts REQUIRED)

  if(UNIX)
    message(STATUS ">>> macOS or Linux")
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

  else()
    message(STATUS ">>> Windows")
  endif()

  add_executable(
    VulkanWrapper
    main.cpp
    src/context.cpp
    src/swapchain.cpp
    src/shader.cpp
    src/render-process.cpp
    src/render.cpp
    src/buffer.cpp
    src/demux-util.cpp)
  target_link_libraries(VulkanWrapper PRIVATE Vulkan::Vulkan glfw glm
                                              cxxopts::cxxopts)
  target_include_directories(VulkanWrapper PRIVATE include)
  target_include_directories(VulkanWrapper PRIVATE ${Stb_INCLUDE_DIR})

  if(UNIX)
    message(STATUS ">>> macOS or Linux")
    target_link_libraries(VulkanWrapper PRIVATE PkgConfig::ffmpeg)
    target_include_directories(VulkanWrapper PRIVATE include)
  else()
    message(STATUS ">>> Windows")
  endif()

  add_executable(VulkanDemo demo.cpp)
  target_link_libraries(VulkanDemo PRIVATE Vulkan::Vulkan glfw glm)
endif(${Vulkan_FOUND})
