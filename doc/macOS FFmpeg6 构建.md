## 配置

> 参数来自于[Static builds for macOS 64-bit](https://evermeet.cx/ffmpeg/)

> 一些依赖库问题可以参考 [Mac M1 编译 FFmpeg6.0](https://juejin.cn/post/7247044423023804476)

```
./configure --cc=/usr/bin/clang --prefix=/Users/icuxika/CommandLineTools/ffmpeg6 --extra-version=tessus --enable-avisynth --enable-fontconfig --enable-gpl --enable-libaom --enable-libass --enable-libbluray --enable-libdav1d --enable-libfreetype --enable-libgsm --enable-libmodplug --enable-libmp3lame --enable-libmysofa --enable-libopencore-amrnb --enable-libopencore-amrwb --enable-libopenh264 --enable-libopenjpeg --enable-libopus --enable-librubberband --enable-libshine --enable-libsnappy --enable-libsoxr --enable-libspeex --enable-libtheora --enable-libtwolame --enable-libvidstab --enable-libvmaf --enable-libvo-amrwbenc --enable-libvorbis --enable-libvpx --enable-libwebp --enable-libx264 --enable-libx265 --enable-libxavs --enable-libxvid --enable-libzimg --enable-libzmq --enable-libzvbi --enable-version3 --pkg-config-flags=--static --disable-ffplay
```

> 安装到`--prefix=/Users/icuxika/CommandLineTools/ffmpeg6`指定的目录

```

make
make install

```

## cmake 配置

```
if(UNIX)
    find_package(PkgConfig REQUIRED)
    set(ENV{PKG_CONFIG_PATH} "/Users/icuxika/CommandLineTools/ffmpeg6/lib/pkgconfig")
    pkg_check_modules(FFMPEG6 REQUIRED IMPORTED_TARGET
        libavcodec
        libavdevice
        libavfilter
        libavformat
        libavutil
        libpostproc
        libswresample
        libswscale
        )
    message(STATUS "${FFMPEG6_INCLUDE_DIRS}")
    message(STATUS "${FFMPEG6_LIBRARY_DIRS}")
else()
endif(UNIX)

add_executable(VulkanWrapper main.cpp)

if(UNIX)
    target_link_libraries(VulkanWrapper PRIVATE PkgConfig::FFMPEG6)
else()
endif(UNIX)
```

有可能会遇到如下错误

```
clang: error: no such file or directory: 'AudioToolbox'
clang: error: no such file or directory: 'CoreAudio'
clang: error: no such file or directory: 'AVFoundation'
clang: error: no such file or directory: 'CoreVideo'
clang: error: no such file or directory: 'CoreMedia'
clang: error: no such file or directory: 'CoreGraphics'
clang: error: no such file or directory: 'OpenGL'
clang: error: no such file or directory: 'CoreImage'
clang: error: no such file or directory: 'AppKit'
clang: error: no such file or directory: 'VideoToolbox'
clang: error: no such file or directory: 'CoreFoundation'
clang: error: no such file or directory: 'CoreServices'
```

这时需要删除之前的构建目录，重新构建项目。

```
rm -rf out
cmake --preset macos-default
cmake --build --preset macos-default-debug
```
