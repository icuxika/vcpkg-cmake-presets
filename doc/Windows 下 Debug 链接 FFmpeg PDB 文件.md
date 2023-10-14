# 使用 ShiftMediaProject FFmpeg 构建出来的静态库

## TODO
- 使用导入库+动态库时候链接 FFmpeg PDB 文件的环境配置

## 拷贝 Debug 版本的静态库到`.\VulkanWrapper\libffmpeg\lib\x64\Debug\`
> 包含以`d.lib`和`d.pdb`为末尾的文件
```shell
Get-ChildItem -Path C:\Users\icuxika\VSCodeProjects\FFmpegWindowsBuild\msvc\lib\x64\ | Where-Object { $_.Name -match "(avcodec|avdevice|avfilter|avformat|avutil|postproc|swresample|swscale).*[d]\.(lib|pdb)$" } | ForEach-Object { Copy-Item -Path $_.FullName -Destination (New-Item -Type Directory -Force .\VulkanWrapper\libffmpeg\lib\x64\Debug) }
```

## VulkanWrapper->CMakeLists.txt 修改 MSVC 部分
```cmake
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(STATUS ">>> Windows MSVC: ${CMAKE_BUILD_TYPE}")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_directories(VulkanWrapper PRIVATE libffmpeg\\lib\\x64\\Debug)
        target_link_libraries(VulkanWrapper PRIVATE debug libavcodecd debug libavformatd debug libavutild debug libswresampled debug libswscaled)
    else ()
        target_link_directories(VulkanWrapper PRIVATE libffmpeg\\lib\\x64)
        target_link_libraries(VulkanWrapper PRIVATE libavcodec libavformat libavutil libswresample libswscale)
    endif ()
endif ()
```

## 构建
> 本项目的`CMakeUserPresets.json`没有在`configurePresets`配置`CMAKE_BUILD_TYPE`，使用静态库的`windows-default-user`时，默认值为`Release`，手动指定`Debug`来链接 Debug 版本的静态库及PDB文件，`不过如果在配置阶段指定了Debug，那么构建Release的时候，要重新以Release类型再次配置项目，不然会遇到error LNK2019错误，另外使用 导入库+动态库的 windows-clang-cl-user 没有这个问题，只需要一次配置，就可以先后构建Debug和Release，如果修改 windows-default-user 也为使用导入库+动态库的方式也不会出现这问题，目前来看，只有使用静态时才会发生这个问题`
### Debug
```shell
cmake --preset windows-default-user -DCMAKE_BUILD_TYPE=Debug
cmake --build --preset windows-default-debug-user --target VulkanWrapper
```

### Release
```shell
cmake --preset windows-default-user -DCMAKE_BUILD_TYPE=Release
cmake --build --preset windows-default-release-user --target VulkanWrapper
```

## CLion 配置
- `CMake options`: `--preset "windows-default-user" -DCMAKE_BUILD_TYPE=Debug`
- `Build directory`: `out\build\windows-default`
- `Build options`: `--preset "windows-default-debug-user"`
