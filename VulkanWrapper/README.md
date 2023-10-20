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

> 注意：如果使用静态库，那么 Release 模式和 Debug 模式不能链接同一个库，具体看[Windows 下 Debug 链接 FFmpeg PDB 文件.md](..%2Fdoc%2FWindows%20%E4%B8%8B%20Debug%20%E9%93%BE%E6%8E%A5%20FFmpeg%20PDB%20%E6%96%87%E4%BB%B6.md)

```
cmake --preset windows-default-user
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

## WSL2 Ubuntu (Preview)

> 因为`Ubuntu 23.10`的软件源包含了 FFmpeg 6， 所以暂时没有尝试在低版本的 Ubuntu 上构建 FFmpeg 6 来使用

```
❯ wsl -v
WSL 版本： 1.2.5.0
内核版本： 5.15.90.1
WSLg 版本： 1.0.51
MSRDC 版本： 1.2.3770
Direct3D 版本： 1.608.2-61064218
DXCore 版本： 10.0.25131.1002-220531-1700.rs-onecore-base2-hyp
Windows 版本： 10.0.22621.2428
```

```
❯ lsb_release -a
No LSB modules are available.
Distributor ID: Ubuntu
Description:    Ubuntu Mantic Minotaur (development branch)
Release:        23.10
Codename:       mantic
```

### 安装 FFmpeg 6 开发库

```
sudo apt install libavcodec-dev libavdevice-dev libavfilter-dev libavformat-dev libavutil-dev libpostproc-dev libswresample-dev libswscale-dev
```

### 安装 Vulkan SDK

> 由于 Vulkan 官方还没有提供通过支持在`Ubuntu 23.10` 上通过 apt 方式安装 SDK 的方式，这里手动构建

[vulkansdk-linux-x86_64-1.3.261.1.tar.xz](https://sdk.lunarg.com/sdk/download/1.3.261.1/linux/vulkansdk-linux-x86_64-1.3.261.1.tar.xz)

#### 解压

```
xz -d vulkansdk-linux-x86_64-1.3.261.1.tar.xz
tar -xvf vulkansdk-linux-x86_64-1.3.261.1.tar -C .
```

#### 构建

> 耗时较久，需要翻墙`export https_proxy=http://192.168.50.88:7890 http_proxy=http://192.168.50.88:7890 all_proxy=http://192.168.50.88:7890`

```
bash vulkansdk
```

#### 使用

> 可以配置到`.zshrc`中，也可以每次构建时临时运行

```
source ~/Source/vulkan/1.3.261.1/setup-env.sh
```

#### 构建遇到的问题

1. 中途会弹出一个配置 postfix 的窗口，如果填写后出现错误，可以尝试以下步骤

```
sudo apt install postfix
sudo vim /etc/postfix/main.cf
```

修改`myhostname`的值使它的格式类似`DESKTOP-EDP10J2.icuxika`

2. python 报错缺少库（由于在安装 Vulkan 之前已经安装了`miniconda3`，所以不确定其他步骤是否也需要 python 环境，使用的默认 base 环境的 python 版本是 3.11.5）

```
pip install jsonschema
```

### 项目编译

> 需要安装一些 glfw3 的依赖库: `sudo apt install libxinerama-dev libxcursor-dev xorg-dev libglu1-mesa-dev`，但是报错会同时误导你`CMAKE_MAKE_PROGRAM`变量没有设置

```
cmake --preset ubuntu-default
cmake --build --preset ubuntu-default-release --target VulkanWrapper
```

#### shader 编译

```
cd VulkanWrapper
bash shaders-generate.sh
```
