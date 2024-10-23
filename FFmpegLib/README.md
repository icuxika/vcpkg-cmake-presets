## 配置依赖库
在`FFmpegLib`目录下执行`.\win-ffmpeg-static-download.ps1`
> [https://github.com/icuxika/FFmpegWindowsBuild](https://github.com/icuxika/FFmpegWindowsBuild) 根据此项目构建出Release和Debug版本的静态`FFmpeg`库


## 构建出静态库
### MSVC 构建（Release）
```shell
cmake --preset windows-default-user
cmake --build --preset windows-default-release-user --target alpha-ffmpeg-static
```

### Clang 构建（Release）
```shell
cmake --preset windows-clang-cl-user
cmake --build --preset windows-clang-cl-release-user --target alpha-ffmpeg-static
```

## 构建出导入库+动态库
### MSVC 构建（Release）
```shell
cmake --preset windows-default-user
cmake --build --preset windows-default-release-user --target alpha-ffmpeg
```

### Clang 构建（Release）
```shell
cmake --preset windows-clang-cl-user
cmake --build --preset windows-clang-cl-release-user --target alpha-ffmpeg
```

## VulkanWrapper 使用
### 静态库
```cmake
target_link_libraries(VulkanWrapper PRIVATE alpha-ffmpeg-static)
```

### 导入库+动态库
```cmake
target_link_libraries(VulkanWrapper PRIVATE alpha-ffmpeg)
```
构建完成后，拷贝`alpha-ffmpeg.dll`到`VulkanWrapper.exe`所在目录