在`Developer Command Prompt for VS 2022`中执行

- [Windows下编译Skia源码的方法](https://github.com/rhett-lee/skia_compile/blob/main/Windows%E4%B8%8B%E7%BC%96%E8%AF%91skia.md)

```shell
set http_proxy=http://127.0.0.1:7890
set https_proxy=http://127.0.0.1:7890
```

## 删除Windows自带的导向Windows商店的python程序

> skia 构建脚本使用了 bat 相关语法，并指定调用`python3`

```shell
rm C:\Users\icuxika\AppData\Local\Microsoft\WindowsApps\python.exe
rm C:\Users\icuxika\AppData\Local\Microsoft\WindowsApps\python3.exe

cp C:\Users\icuxika\miniconda3\python.exe C:\Users\icuxika\miniconda3\python3.exe
```

## 构建 [gn](https://gn.googlesource.com/gn/+/refs/heads/main)

```shell
git clone https://gn.googlesource.com/gn
cd gn
python build/gen.py --allow-warning
ninja -C out
```

## [How to download Skia](https://skia.org/docs/user/download/)

```shell
git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
set PATH=%PATH%;C:\Users\icuxika\source\repos\depot_tools

git clone https://skia.googlesource.com/skia.git
cd skia
python3 tools/git-sync-deps
python3 bin/fetch-ninja
```

## [How to build Skia](https://skia.org/docs/user/build/#windows)

> 构建时如果提示找不到`gn.exe`，那就将上述自行构建的`gn.exe`拷贝到目录`buildtools\win`

> 当前`skia`的完整构建能够使用，第二条禁用了许多组件的构建命令还需验证

```shell
gn gen out/vs2022.x64.Release --ide="vs2022" --sln="skia" --args="clang_win=\"C:\\CommandLineTools\\SourceInstall\\llvm\" cc=\"clang\" cxx=\"clang++\" extra_cflags=[\"/MT\"] is_debug=false"

gn gen out/vs2022.x64.Release --ide="vs2022" --sln="skia" --args="clang_win=\"C:\\CommandLineTools\\SourceInstall\\llvm\" cc=\"clang\" cxx=\"clang++\" extra_cflags=[\"-DSK_DISABLE_LEGACY_PNG_WRITEBUFFER\",\"/MT\"] is_debug=false is_official_build=true is_trivial_abi=false skia_enable_fontmgr_win_gdi=false skia_enable_pdf=false skia_use_expat=false skia_use_icu=false skia_use_libjpeg_turbo_decode=false skia_use_libjpeg_turbo_encode=false skia_use_libpng_decode=false skia_use_libpng_encode=false skia_use_libwebp_decode=false skia_use_libwebp_encode=false skia_use_wuffs=false skia_use_xps=false skia_use_zlib=false"
gn args out/vs2022.x64.Release --list
ninja -C out/vs2022.x64.Release
```

## 使用

- 当使用`MSVC`编译程序时，在`#include <windows.h>`前添加`#define NOMINMAX`避免min、max函数模板冲突
- 拷贝`skia`源码目录的`include`目录到本项目模块下的`skia`目录下
- 拷贝`out\vs2022.x64.Release`目录下的`skia.lib`到本项目模块下的`skia\lib`目录下

## 补充

- `gn gen`参数详情 `gn gen --help`
- 构建配置详情 `gn args out/vs2022.x64.Release --list`