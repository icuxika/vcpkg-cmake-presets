# Windows FFmpeg 构建

[ShiftMediaProject](https://github.com/ShiftMediaProject/FFmpeg) 是非官方的 FFmpeg 构建项目，支持使用 Visual Studio 2022 构建出 windows 平台下的静态库和动态库，
相关教程 [vs2019 编译 ffmpeg 源码为静态库动态库【完整步骤、亲测可行】](https://blog.csdn.net/yao_hou/article/details/121581878?utm_medium=distribute.pc_relevant.none-task-blog-2~default~baidujs_baidulandingword~default-0-121581878-blog-109981238.235^v38^pc_relevant_anti_vip_base&spm=1001.2101.3001.4242.1&utm_relevant_index=3)，如果不想使用 FFmpeg 官网提供的链接上关联的必须有动态库的构建包，建议直接使用此项目提供的构建包，下面是在 MSYS2 中构建 FFmpeg，但是静态构建只能构建出`.a`格式的静态，难以在 Windows 环境下使用

### 环境 MSYS2 CLANG64

1. Developer PowerShell for VS 2022 (在 Visual Studio 提供的命令行环境中启动 MSYS2)
2. MSYS2

```
C:/msys64/msys2_shell.cmd -defterm -here -no-start -clang64 -shell zsh
```

3. 将 MSVC 编译器路径添加到临时环境变量

```
export PATH="/c/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.37.32822/bin/Hostx64/x64":$PATH
```

> 此时 MSYS2 环境已经可以使用 MSVC 的 cl.exe 编译器了，如果没有从`Developer PowerShell for VS 2022`进入 msys2 的话，即使你添加了这个环境变量，ffmpeg 的构建也会提示你`C compiler test failed.`

### 一些依赖

```
pacman -S mingw-w64-clang-x86_64-toolchain
pacman -S msys/make
pacman -S clang64/mingw-w64-clang-x86_64-nasm clang64/mingw-w64-clang-x86_64-yasm clang64/mingw-w64-clang-x86_64-diffutils
```

### x264

> 不要同时 --enable-shared

```
git clone https://code.videolan.org/videolan/x264.git
cd x264
CC=cl ./configure --enable-static
make
make install
```

### ffmpeg

```
mkdir Source
cd Source
curl -O https://ffmpeg.org/releases/ffmpeg-6.0.tar.xz
xz -d ffmpeg-6.0.tar.xz
tar -xvf ffmpeg-6.0.tar -C .
cp -r ffmpeg-6.0 ffmpeg-6.0-static
cp -r ffmpeg-6.0 ffmpeg-6.0-shared
```

#### ffmpeg static

> 静态编译，目前编译出来的 ffmpeg.exe 依赖 libx264-164.dll，通过静态连接 libx264 失败了

```
cd ffmpeg-6.0-static
PKG_CONFIG_PATH=/usr/local/lib/pkgconfig ./configure --target-os=win64 --arch=x86_64 --toolchain=msvc --prefix=/c/CommandLineTools/SourceInstall/ffmpeg6-static --enable-static --enable-libx264 --enable-gpl --pkg-config-flags=--static
make -j16
make install
```

#### ffmpeg shared

> 动态编译，但是生成的 lib 文件都在 bin 目录下

```
cd ffmpeg-6.0-shared
PKG_CONFIG_PATH=/usr/local/lib/pkgconfig ./configure --target-os=win64 --arch=x86_64 --toolchain=msvc --prefix=/c/CommandLineTools/SourceInstall/ffmpeg6-shared --enable-libx264 --enable-gpl --disable-static --enable-shared --pkg-config-flags=--static
make -j16
make install
```
