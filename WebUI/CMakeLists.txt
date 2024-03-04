add_executable(webuiMinimal main.cpp)
target_include_directories(webuiMinimal PRIVATE webui-lib\\webui-windows-msvc-x64\\include)
target_link_directories(webuiMinimal PRIVATE webui-lib\\webui-windows-msvc-x64)
target_link_libraries(webuiMinimal PRIVATE webui-2-static user32 Advapi32 Shell32)