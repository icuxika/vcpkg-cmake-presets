### 安装clang
```
sudo apt install clang
```

### 示例 `CMakeUserPresets.json`
> 仅修改了`CMakeUserPresets.json.template`的`default-user`配置以用于Ubuntu
```
{
    "version": 4,
    "include": [
        "CMakePresets.json"
    ],
    "configurePresets": [
        {
            "name": "default-user",
            "inherits": "default",
            "generator": "Ninja Multi-Config",
            "cacheVariables": {
                "CMAKE_TOOLCHAIN_FILE": {
                    "value": "/home/icuxika/vcpkg/scripts/buildsystems/vcpkg.cmake",
                    "type": "FILEPATH"
                },
                "CMAKE_CXX_FLAGS": "",
                "CMAKE_C_COMPILER": "clang",
                "CMAKE_CXX_COMPILER": "clang++"
            }
        },
        {
            "name": "clang-cl-user",
            "inherits": "clang-cl",
            "environment": {
                "VCPKG_ROOT": "C:/CommandLineTools/vcpkg"
            }
        },
        {
            "name": "mingw-clang64-user",
            "inherits": "mingw-clang64",
            "environment": {
                "VCPKG_ROOT": "C:/CommandLineTools/vcpkg",
                "MSYS_PATH": "C:/msys64",
                "MSYS_SUBSYSTEM": "clang64"
            }
        }
    ],
    "buildPresets": [
        {
            "name": "default-debug-user",
            "configurePreset": "default-user",
            "configuration": "Debug"
        },
        {
            "name": "default-release-user",
            "configurePreset": "default-user",
            "configuration": "Release"
        },
        {
            "name": "clang-cl-debug-user",
            "configurePreset": "clang-cl-user",
            "configuration": "Debug"
        },
        {
            "name": "clang-cl-release-user",
            "configurePreset": "clang-cl-user",
            "configuration": "Release"
        },
        {
            "name": "mingw-clang64-debug-user",
            "configurePreset": "mingw-clang64-user",
            "configuration": "Debug"
        }
    ]
}
```

### 构建
```
cmake --preset clang-cl-user
cmake --build --preset clang-cl-debug
```

### 可能遇到的错误
> 如果项目构建遇到了错误`cannot find -lstdc++: no such file or directory`，那么需要查看`clang -v`的`Selected GCC installation`
```
> clang -v
Ubuntu clang version 14.0.0-1ubuntu1.1
Target: x86_64-pc-linux-gnu
Thread model: posix
InstalledDir: /usr/bin
Found candidate GCC installation: /usr/bin/../lib/gcc/x86_64-linux-gnu/11
Found candidate GCC installation: /usr/bin/../lib/gcc/x86_64-linux-gnu/12
Selected GCC installation: /usr/bin/../lib/gcc/x86_64-linux-gnu/12
Candidate multilib: .;@m64
Selected multilib: .;@m64
Found CUDA installation: /usr/local/cuda-11.7, version
```

> 安装以下包后重新构建，注意数字`12`与`/usr/bin/../lib/gcc/x86_64-linux-gnu/12`相对应
```
sudo apt install g++-12
```
