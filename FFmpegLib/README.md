## 配置依赖库
1. 在`FFmpegLib`目录下执行`.\win-ffmpeg-static-download.ps1`
2. 精简尺寸的静态库
> [https://github.com/icuxika/FFmpegWindowsBuild](https://github.com/icuxika/FFmpegWindowsBuild) 根据此项目构建出Release和Debug版本的静态`FFmpeg`库
```shell
New-Item -ItemType Directory -Path .\libffmpeg\lib\x64\Customized
Get-ChildItem -Path C:\Users\icuxika\VSCodeProjects\FFmpegWindowsBuild\msvc\lib\x64\ -Filter "*.lib" | Where-Object { $_.Name -match "libavcodec\.lib|libavdevice\.lib|libavfilter\.lib|libavformat\.lib|libavutil\.lib|libpostproc\.lib|libswresample\.lib|libswscale\.lib" } | ForEach-Object { Copy-Item -Path $_.FullName -Destination .\libffmpeg\lib\x64\Customized }

New-Item -ItemType Directory -Path .\libffmpeg\lib\x64\Debug
Get-ChildItem -Path C:\Users\icuxika\VSCodeProjects\FFmpegWindowsBuild\msvc\lib\x64\ | Where-Object { $_.Name -match "(avcodec|avdevice|avfilter|avformat|avutil|postproc|swresample|swscale).*[d]\.(lib|pdb)$" } | ForEach-Object { Copy-Item -Path $_.FullName -Destination .\libffmpeg\lib\x64\Debug }
```

## 构建
> `win-ffmpeg-static-download.ps1`下载的库的静态版本不支持Clang，而通过`FFmpegWindowsBuild`自行构建的静态库支持Clang
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