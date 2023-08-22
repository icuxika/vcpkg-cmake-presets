vcpkg 清单模式 + cmake-presets 来构建C++项目
==========

在Windows上使用vcpkg和CMakePresets来构建C++项目

## 相关命令
```
cmake --list-presets=all .
cmake --preset <configurePreset-name>
cmake --build --preset <buildPreset-name> 
```

## 执行
### `default`
```
cmake --preset default-user
cmake --build --preset default-release
cmake --build --preset default-release --target install
```
### `clang-cl`（依赖Visual Studio提供的命令行环境）
```
cmake --preset clang-cl-user
cmake --build --preset clang-cl-release
cmake --build --preset clang-cl-release --target install
```

## vcpkg
由于使用的的清单模式，所以只需要安装，而不需要使用`.\vcpkg\vcpkg install [packages to install]`去主动安装软件包，`CMakePresets.json`中指定了`vcpkg`作为`toolchain`后，`vcpkg`会将`vcpkg.json`中声明的依赖安装到`${binaryDir}/vcpkg_installed`中。

### 安装
```
git clone https://github.com/microsoft/vcpkg
.\vcpkg\bootstrap-vcpkg.bat
```

## CMakePresets
### Power Shell 构建记录
> 此构建使用随`Visual Studio`安装的`clang-cl`进行构建，需要在`Visual Studio`提供命令行环境中运行，当前使用的是`Windows终端`自动检索到的`Developer PowerShell for VS2022`，若要在`vscode`中开发，需要从此命令行以`code .`打开`vscode`从而能够集成`Visual Studio`的环境
```
>> cmake --list-presets=all .
Available configure presets:

  "default-user"
  "clang-cl-user"
  "default"
  "clang-cl"

Available build presets:

  "default-debug"
  "default-release"
  "clang-cl-debug"
  "clang-cl-release"

>> cmake --preset clang-cl-user
Preset CMake variables:

  CMAKE_CXX_COMPILER="clang-cl"
  CMAKE_C_COMPILER="clang-cl"
  CMAKE_INSTALL_PREFIX="C:/Users/icuxika/VSCodeProjects/vcpkg-cmake-presets/out/install/clang-cl"
  CMAKE_TOOLCHAIN_FILE:FILEPATH="C:/CommandLineTools/vcpkg/scripts/buildsystems/vcpkg.cmake"

-- Running vcpkg install
Detecting compiler hash for triplet x64-windows...
-- Automatically setting HTTP(S)_PROXY environment variables to "127.0.0.1:7890".
The following packages will be built and installed:
    cxxopts:x64-windows -> 3.1.1
    fmt:x64-windows -> 10.0.0
    range-v3:x64-windows -> 0.12.0#1
  * vcpkg-cmake:x64-windows -> 2023-05-04
  * vcpkg-cmake-config:x64-windows -> 2022-02-06#1
Additional packages (*) will be modified to complete this operation.
Restored 5 package(s) from C:\Users\icuxika\AppData\Local\vcpkg\archives in 133 ms. Use --debug to see more details.
Installing 1/5 vcpkg-cmake-config:x64-windows...
Elapsed time to handle vcpkg-cmake-config:x64-windows: 2.92 ms
Installing 2/5 vcpkg-cmake:x64-windows...
Elapsed time to handle vcpkg-cmake:x64-windows: 2.97 ms
Installing 3/5 cxxopts:x64-windows...
Elapsed time to handle cxxopts:x64-windows: 3.25 ms
Installing 4/5 fmt:x64-windows...
Elapsed time to handle fmt:x64-windows: 7.66 ms
Installing 5/5 range-v3:x64-windows...
Elapsed time to handle range-v3:x64-windows: 52.2 ms
Total install time: 69 ms
cxxopts provides CMake targets:

    # this is heuristically generated, and may not be correct
    find_package(cxxopts CONFIG REQUIRED)
    target_link_libraries(main PRIVATE cxxopts::cxxopts)

The package fmt provides CMake targets:

    find_package(fmt CONFIG REQUIRED)
    target_link_libraries(main PRIVATE fmt::fmt)

    # Or use the header-only version
    find_package(fmt CONFIG REQUIRED)
    target_link_libraries(main PRIVATE fmt::fmt-header-only)

range-v3 provides CMake targets:

    # this is heuristically generated, and may not be correct
    find_package(range-v3 CONFIG REQUIRED)
    target_link_libraries(main PRIVATE range-v3::meta range-v3::concepts range-v3::range-v3)

-- Running vcpkg install - done
-- The C compiler identification is Clang 15.0.1 with MSVC-like command-line
-- The CXX compiler identification is Clang 15.0.1 with MSVC-like command-line
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/x64/bin/clang-cl.exe - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/x64/bin/clang-cl.exe - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done (15.7s)
-- Generating done (0.0s)
-- Build files have been written to: C:/Users/icuxika/VSCodeProjects/vcpkg-cmake-presets/out/build/clang-cl

>> cmake --build --preset clang-cl-release
[2/2] Linking CXX executable Release\VcpkgCmakePresets.exe

>> cmake --build --preset clang-cl-release --target install
[0/1] Install the project...-- Install configuration: "Release"
-- Installing: C:/Users/icuxika/VSCodeProjects/vcpkg-cmake-presets/out/install/clang-cl/Release/bin/VcpkgCmakePresets.exe
```

### 使用 MSYS2 作为工具链
> 主要使用`clang64`环境，`ucrt64`和`mingw64`可以通过修改`CMakePresets.json`对应配置项的`MSYS_SUBSYSTEM`来设置，目前来说`clang64`遇到的问题比较容易解决
```
cmake --preset mingw-clang64-user
cmake --build --preset mingw-clang64-debug-user
```
构建好的程序在`MSYS2`环境中可以直接运行，通过`ldd out/build/mingw-clang64-user/Debug/VcpkgCmakePresets.exe`可以得到
```
➜  vcpkg-cmake-presets git:(main) ✗ ldd out/build/mingw-clang64-user/Debug/VcpkgCmakePresets.exe
        ntdll.dll => /c/Windows/SYSTEM32/ntdll.dll (0x7ffc91390000)
        KERNEL32.DLL => /c/Windows/System32/KERNEL32.DLL (0x7ffc903f0000)
        KERNELBASE.dll => /c/Windows/System32/KERNELBASE.dll (0x7ffc8e770000)
        ucrtbase.dll => /c/Windows/System32/ucrtbase.dll (0x7ffc8ef90000)
        libc++.dll => /clang64/bin/libc++.dll (0x7ffbdf3f0000)
```
其中`libc++.dll`所在路径由于没有配置到环境变量中，手动拷贝到 exe 所在目录下后能够在`Windows 终端`正常运行。

通过在`Developer PowerShell for VS2022`环境中运行`dumpbin.exe /dependents .\out\build\mingw-clang64-user\Debug\VcpkgCmakePresets.exe`可以得到
```
Dump of file .\out\build\mingw-clang64-user\Debug\VcpkgCmakePresets.exe

File Type: EXECUTABLE IMAGE

  Image has the following dependencies:

    KERNEL32.dll
    libc++.dll
    api-ms-win-crt-heap-l1-1-0.dll
    api-ms-win-crt-private-l1-1-0.dll
    api-ms-win-crt-runtime-l1-1-0.dll
    api-ms-win-crt-stdio-l1-1-0.dll
    api-ms-win-crt-string-l1-1-0.dll
    api-ms-win-crt-math-l1-1-0.dll
    api-ms-win-crt-environment-l1-1-0.dll
    api-ms-win-crt-time-l1-1-0.dll
    api-ms-win-crt-utility-l1-1-0.dll

  Summary

        1000 .00cfg
        1000 .buildid
        1000 .data
        3000 .debug_abbrev
        1000 .debug_aranges
       99000 .debug_info
       44000 .debug_line
        5000 .debug_loc
       12000 .debug_ranges
      12C000 .debug_str
        C000 .pdata
       15000 .rdata
        1000 .reloc
       77000 .text
        1000 .tls
```
`ucrt64`和`mingw64`编译出的程序依赖关系相对比较复杂些，如果需要使用，需要仔细处理，目前来看，还是使用`Visual Studio`提供的编译工具使用更为方便

#### 使用MSYS2时进行Debug (由于我没有将MSYS2相关的路径添加到环境变量，所以这些程序都需要根据vscode手动配置，同时也方便切换MSYS2中的不同环境进行测试)
需要在 `.vscode/settings.json`中指定下`C:/msys64/clang64/bin/gdb.exe`，这个路径取决于使用的MSYS2环境，使用`Visual Studio`提供的编译工具时，Debug不需要此配置，所以目前命名为`settings.json.template`防止生效，使用时要去掉文件后缀

### CMake Tools 存在的问题
对比`CMakePresets.json`与`CMakeUserPresets.json`发现，当计算机上没有指定环境变量`VCPKG_ROOT`时，使用特定于某个用户的`CMakeUserPresets.json`文件补充了`vcpkg`路径，但是当在`VS Code`中选择了`CMakeUserPresets.json`的`default-user`时，底部的`CMake Tools`状态栏无法选择在`CMakePresets.json`填写的`build presets`，但是在命令行使用cmake相关命令却没有问题，目前项目使用的配置都较为简陋，虽然在底部的`CMake Tools`状态栏无法选择`build presets`，但是默认生成的是`Debug`版本能够满足开发使用，若要获得提示，需要将在`CMakePresets.json`填写的`build presets`复制到`CMakeUserPresets.json`中并更新`configurePreset`相关字段。

### git
`CMakePresets.json`应该被`git`管理，`CMakeUserPresets.json`应该不被`git`管理，后者只供开发人员用来保存自己的本地生成，但本项目为了知道基本的使用，保存了一份` CMakeUserPresets.json.template`。

## 与Conan2.0的对比
> 使用conan一般需要先执行`conan install . -s build_type=Debug --output-folder=cmake-build-debug --build=missing`命令来安装依赖，同时conan会自动生成`CMakePresets.json`在构建目录（`cmake-build-debug`）下，生成`CMakeUserPresets.json`在项目根目录下并同时在文件中 include `CMakePresets.json`

> vcpkg需要手动在项目根目录下创建`CMakePresets.json`，`CMakeUserPresets.json`两个文件

> conan在`conan install`执行时安装依赖，vcpkg在`cmake --preset <configurePreset-name>`执行时安装依赖