# 基于 Vulkan 的 Yuv420P 播放器

## Windows

### 编译

> 使用随 Visual Studio 一起安装的 clang-cl 编译套件（在 Visual Studio 提供的命令行环境中运行以下命令）

```
cmake --preset windows-clang-cl-user
cmake --build --preset windows-clang-cl-debug-user
cmake --build --preset windows-clang-cl-release-user
```

### 拷贝 shader 文件到可执行程序所在目录下的 shader 目录下

```
cp -Recurse .\VulkanWrapper\shaders-bin\ .\out\build\windows-clang-cl\VulkanWrapper\Debug\shaders
```

### 拷贝相关动态库到可执行程序所在目录下

```
cp C:\CommandLineTools\ffmpeg-6.0-full_build-shared\bin\*.dll .\out\build\windows-clang-cl\VulkanWrapper\Debug\
```

### 拷贝 纹理图片 到可执行程序所在目录下的 textures 目录下

```
cp -Recurse .\VulkanWrapper\textures\ .\out\build\windows-clang-cl\VulkanWrapper\Debug\
```

### 运行

```
cd .\out\build\windows-clang-cl\VulkanWrapper\Debug\
.\VulkanWrapper.exe -f C:\Users\icuxika\Desktop\test.mp4
```

### 注意

Vulkan 与 MSI Afterburner 有冲突，可能会导致 Vulkan 中 SwapChain 的创建出现奇怪问题，MSI Afterburner 即使不开机启动也会后台跑一个进程，需要在任务管理器杀掉

## Windows MSVC

> 不需要在 Visual Studio 提供的命令行环境中运行

```
cmake --preset windows-default-user
cmake --build --preset windows-default-debug-user

cp -Recurse .\VulkanWrapper\shaders-bin\ .\out\build\windows-default\VulkanWrapper\Debug\shaders
cp C:\CommandLineTools\ffmpeg-6.0-full_build-shared\bin\*.dll .\out\build\windows-default\VulkanWrapper\Debug\
cp -Recurse .\VulkanWrapper\textures\ .\out\build\windows-default\VulkanWrapper\Debug\

cd .\out\build\windows-default\VulkanWrapper\Debug\
.\VulkanWrapper.exe -f C:\Users\icuxika\Desktop\test.mp4
```

## macOS

```
cmake --preset macos-default
cmake --build --preset macos-default-debug
cmake --build --preset macos-default-release
```
