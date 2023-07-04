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
```
cmake --preset vcpkg-default-user
cmake --build --preset vcpkg-release
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
```
>> cmake --list-presets=all .
Available configure presets:

  "vcpkg-default-user"
  "vcpkg-default"

Available build presets:

  "vcpkg-debug"
  "vcpkg-release"

>> cmake --preset vcpkg-default-user
Preset CMake variables:

  CMAKE_TOOLCHAIN_FILE:FILEPATH="C:/CommandLineTools/vcpkg/scripts/buildsystems/vcpkg.cmake"

-- Building for: Visual Studio 17 2022
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
Restored 5 package(s) from C:\Users\icuxika\AppData\Local\vcpkg\archives in 137 ms. Use --debug to see more details.
Installing 1/5 vcpkg-cmake-config:x64-windows...
Elapsed time to handle vcpkg-cmake-config:x64-windows: 2.93 ms
Installing 2/5 vcpkg-cmake:x64-windows...
Elapsed time to handle vcpkg-cmake:x64-windows: 3.23 ms
Installing 3/5 cxxopts:x64-windows...
Elapsed time to handle cxxopts:x64-windows: 3.53 ms
Installing 4/5 fmt:x64-windows...
Elapsed time to handle fmt:x64-windows: 8.75 ms
Installing 5/5 range-v3:x64-windows...
Elapsed time to handle range-v3:x64-windows: 51 ms
Total install time: 69.5 ms
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
-- The C compiler identification is MSVC 19.36.32535.0
-- The CXX compiler identification is MSVC 19.36.32535.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.36.32532/bin/Hostx64/x64/cl.exe - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.36.32532/bin/Hostx64/x64/cl.exe - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Configuring done (6.1s)
-- Generating done (0.0s)
-- Build files have been written to: C:/Users/icuxika/VSCodeProjects/vcpkg-cmake-presets/cmake-build-debug

>> cmake --build --preset vcpkg-release
MSBuild version 17.6.3+07e294721 for .NET Framework

  1>Checking Build System
  Building Custom Rule C:/Users/icuxika/VSCodeProjects/vcpkg-cmake-presets/CMakeLists.txt
  main.cpp
  VcpkgCmakePresets.vcxproj -> C:\Users\icuxika\VSCodeProjects\vcpkg-cmake-presets\cmake-build-debug\Release\VcpkgCmake
  Presets.exe
  Building Custom Rule C:/Users/icuxika/VSCodeProjects/vcpkg-cmake-presets/CMakeLists.txt
>> 
```
### CMake Tools 存在的问题
对比`CMakePresets.json`与`CMakeUserPresets.json`发现，当计算机上没有指定环境变量`VCPKG_ROOT`时，使用特定于某个用户的`CMakeUserPresets.json`文件补充了`vcpkg`路径，但是当在`VS Code`中选择了`CMakeUserPresets.json`的`vcpkg-default-user`时，底部的`CMake Tools`状态栏无法选择在`CMakePresets.json`填写的`build presets`，但是在命令行使用cmake相关命令却没有问题。

### git
`CMakePresets.json`应该被`git`管理，`CMakeUserPresets.json`应该不被`git`管理，后者只供开发人员用来保存自己的本地生成，但本项目为了知道基本的使用，保存了一份` CMakeUserPresets.json.template`。

## 与Conan2.0的对比
> 使用conan一般需要先执行`conan install . -s build_type=Debug --output-folder=cmake-build-debug --build=missing`命令来安装依赖，同时conan会自动生成`CMakePresets.json`在构建目录（`cmake-build-debug`）下，生成`CMakeUserPresets.json`在项目根目录下并同时在文件中 include `CMakePresets.json`

> vcpkg需要手动在项目根目录下创建`CMakePresets.json`，`CMakeUserPresets.json`两个文件

> conan在`conan install`执行时安装依赖，vcpkg在`cmake --preset <configurePreset-name>`执行时安装依赖