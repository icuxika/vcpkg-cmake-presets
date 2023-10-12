# 基于 Vulkan 的 Yuv420P 播放器

## 下载 Vulkan SDK

[Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows)，`VulkanSDK-1.3.261.1-Installer.exe`安装程序会自动配置环境变量，之后 cmake 就可以通过`find_package(Vulkan REQUIRED)`来查找它了

## 下载 FFmpeg 库

> 下载 [ShiftMediaProject 提供的 FFmpeg 构建包，同时提供静态与动态构建版本](https://github.com/ShiftMediaProject/FFmpeg/releases)

```
cd .\VulkanWrapper\
.\win-ffmpeg-static-download.ps1
cd ..
```

#### 由于从 GitHub 下载可能需要配置 PowerShell VPN

```
$env:HTTP_PROXY = "http://127.0.0.1:7890"
$env:HTTPS_PROXY = "http://127.0.0.1:7890"
```

## shader 编译

> cmake 构建时会自动拷贝该目录到可执行程序目录下

```
cd .\VulkanWrapper\
.\shaders-generate.ps1
cd ..
```

## Windows MSVC

> 不需要在 Visual Studio 提供的命令行环境中运行，`链接的静态库，不用拷贝动态库到可执行程序目录下`

### 编译

```
cmake --preset windows-default-user
cmake --build --preset windows-default-debug-user --target VulkanWrapper
cmake --build --preset windows-default-release-user --target VulkanWrapper
```

### 运行

```
cd .\out\build\windows-default\VulkanWrapper\Release\
.\VulkanWrapper.exe -f C:\Users\icuxika\Desktop\test.mp4
```

## Windows Clang

### 编译

> 使用随 Visual Studio 一起安装的 clang-cl 编译套件（在 Visual Studio 提供的命令行环境中运行以下命令）

```
cmake --preset windows-clang-cl-user
cmake --build --preset windows-clang-cl-debug-user --target VulkanWrapper
cmake --build --preset windows-clang-cl-release-user --target VulkanWrapper
```

### 拷贝相关动态库到可执行程序所在目录下

```
cp .\VulkanWrapper\libffmpeg\bin\x64\*.dll .\out\build\windows-clang-cl\VulkanWrapper\Release\
```

### 运行

```
cd C:\Users\icuxika\VSCodeProjects\vcpkg-cmake-presets\
.\VulkanWrapper.exe -f C:\Users\icuxika\Desktop\test.mp4
```

### 注意

Vulkan 与 MSI Afterburner 有冲突，可能会导致 Vulkan 中 SwapChain 的创建出现奇怪问题，MSI Afterburner 即使不开机启动也会后台跑一个进程，需要在任务管理器杀掉

## macOS

```
cmake --preset macos-default
cmake --build --preset macos-default-debug --target VulkanWrapper
cmake --build --preset macos-default-release --target VulkanWrapper
```
