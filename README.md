vcpkg 清单模式 + cmake-presets 来构建C++项目
==========


# cmake-presets 相关命令，[文档](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html)
```
cmake --list-presets=all .
cmake --preset <configurePreset-name>
cmake --build --preset <buildPreset-name> 
```

# vcpkg 安装
> 由于使用的的清单模式，所以只需要安装，而不需要使用`.\vcpkg\vcpkg install [packages to install]`去主动安装软件包，`CMakePresets.json`中指定了`vcpkg`作为`toolchain`后，`vcpkg`会将`vcpkg.json`中声明的依赖安装到`${binaryDir}/vcpkg_installed`中。
```
git clone https://github.com/microsoft/vcpkg
.\vcpkg\bootstrap-vcpkg.bat
./vcpkg/bootstrap-vcpkg.sh
```

# 不同平台构建
## Windows
```
cp .\cmake\CMakeUserPresets.json.windows.template .\CMakeUserPresets.json
```
#### windows-default-user
> 使用默认的 cl 编译器
```
cmake --preset windows-default-user
cmake --build --preset windows-default-release-user
cmake --build --preset windows-default-release-user --target install
```

#### windows-clang-cl-user
> 使用随 Visual Studio 安装的 clang 编译器
```
cmake --preset windows-clang-cl-user
cmake --build --preset windows-clang-cl-release-user
cmake --build --preset windows-clang-cl-release-user --target install
```

#### windows-msys2-user
> 使用 MSYS2 中的 编译器
```
cmake --preset windows-msys2-user
cmake --build --preset windows-msys2-release-user
cmake --build --preset windows-msys2-release-user --target install
```

## Ubuntu
```
cp cmake/CMakeUserPresets.json.ubuntu.template CMakeUserPresets.json
```

#### ubuntu-default
> 使用 apt 安装的 build-essential 提供的编译器
```
cmake --preset ubuntu-default
cmake --build --preset ubuntu-default-debug
cmake --build --preset ubuntu-default-debug --target install
```

#### clang
> 使用 apt 安装的 clang编译器
```
cmake --preset clang
cmake --build --preset clang-debug
cmake --build --preset clang-debug --target install
```

# 分析可执行程序依赖的动态库
## Windows
> 使用 Visual Studio Developer PowerShell（安装了VS的话，Windows 终端标签页下拉列表会自动添加此配置，开始菜单`Visual Studio 2022`下也可以找到）
```
dumpbin.exe /dependents .\out\build\windows-clang-cl\Debug\VcpkgCmakePresets.exe
```
可以得到输出
```
Microsoft (R) COFF/PE Dumper Version 14.37.32822.0
Copyright (C) Microsoft Corporation.  All rights reserved.


Dump of file .\out\build\windows-clang-cl\Debug\VcpkgCmakePresets.exe

File Type: EXECUTABLE IMAGE

  Image has the following dependencies:

    KERNEL32.dll
    MSVCP140D.dll
    VCRUNTIME140D.dll
    VCRUNTIME140_1D.dll
    ucrtbased.dll

  Summary

        1000 .00cfg
        2000 .data
        C000 .pdata
       17000 .rdata
        1000 .reloc
        1000 .rsrc
       82000 .text
        1000 .tls
```

## Linux（Ubuntu、MSYS2）

#### Ubuntu
```
❯ ldd out/build/clang/Debug/VcpkgCmakePresets
        linux-vdso.so.1 (0x00007fffb35dc000)
        libstdc++.so.6 => /lib/x86_64-linux-gnu/libstdc++.so.6 (0x00007fa25b805000)
        libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007fa25b71e000)
        libgcc_s.so.1 => /lib/x86_64-linux-gnu/libgcc_s.so.1 (0x00007fa25b6fe000)
        libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007fa25b4d6000)
        /lib64/ld-linux-x86-64.so.2 (0x00007fa25baf5000)
```

#### MSYS2 CLANG64
> 为了不将`MSYS2`的路径添加到Windows的系统环境变量中，`CMakePresets.json` 中通过在`environment`指定`PATH`来加载`MSYS2`提供的编译环境，从而可以正常使用`MSYS2`提供的编译环境编译项目，不过根据下方输出结果，由于`/clang64/bin/libc++.dl`(Windows路径为`C:\msys64\clang64\bin\libc++.dll`)所在路径不在系统环境变量中，在命令行单独执行构建好的程序时，需要先将这个库拷贝到程序所在目录下，`cp /clang64/bin/libc++.dll out/build/windows-msys2/Debug`
```
➜  vcpkg-cmake-presets git:(main) ✗ ldd out/build/windows-msys2/Debug/VcpkgCmakePresets.exe
        ntdll.dll => /c/Windows/SYSTEM32/ntdll.dll (0x7fff66a70000)
        KERNEL32.DLL => /c/Windows/System32/KERNEL32.DLL (0x7fff65dd0000)
        KERNELBASE.dll => /c/Windows/System32/KERNELBASE.dll (0x7fff64450000)
        apphelp.dll => /c/Windows/SYSTEM32/apphelp.dll (0x7fff5fb60000)
        ucrtbase.dll => /c/Windows/System32/ucrtbase.dll (0x7fff64170000)
        libc++.dll => /clang64/bin/libc++.dll (0x7ffe932b0000)
```

# 其他
使用 `vscode` 开发时一些插件配置和 `Ubuntu` 上编译遇到的问题记录可以查看 `doc` 目录下的文件


