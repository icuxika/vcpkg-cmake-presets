VSCode使用MSYS2编译C++程序时对 CMake Tools 插件的一些配置
==========

### 说明
未在Windows系统环境变量`PATH`中添加任何`MSYS2`环境相关的路径，因为装东西的比较多，不同软件不用版本的依赖库同时被检索到很容易引起冲突，以下的配置都是为了让vscode或者是相关插件能够检索到编译环境

本项目使用了 [cmake-presets](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html) 和 [vcpkg](https://github.com/microsoft/vcpkg/blob/master/README_zh_CN.md) 来开发C++项目，但是以下配置对于一个简单的CMake项目 (比如只有`CMakeLists.txt`和`main.cpp`两个文件的项目) 也是同样适用

### vscode用户设置
> 使插件能够检测到`MSYS2`中的编译器，`插件检测到了相关编译环境(环境包含了程序所需要的依赖)，所以通过插件界面运行项目时，程序都能检索到相关的依赖库，能够正常运行，但是构建后的项目直接通过终端工具执行时可能会出问题，因为此时终端工具并没有检索到相关环境，这个问题需要通过分析exe的依赖库或者将MSYS2的环境添加到系统环境变量中来得到解决，此处可以与README中的对比看`
```
"cmake.additionalCompilerSearchDirs": [
    "C:/msys64/mingw64/bin",
    "C:/msys64/clang64/bin",
    "C:/msys64/ucrt64/bin"
],
```

### 项目根目录下`.vscode\settings.json`
> 指定使用`MSYS2`下的`cmake`程序，`debug`时传递主函数参数，`miDebuggerPath`用来指定使用的`gdb`程序，必须使用`"C:\\msys64\\mingw64\\bin\\gdb.exe"`或者`C:/msys64/mingw64/bin/gdb.exe`这样的路径，即包含`.exe`后缀，不然会有`The value of miDebuggerPath is invalid problem`的插件报错，`gdb程序如果不手动指定，它默认会去调用Visual Studio提供的环境，此处gdb路径要根据使用的MSYS2环境不同切换，如果Windows本身有安装CMake程序，那么cmake.cmakePath不指定也没关系，使用Windows下的CMake程序也能正常构建项目`
```
{
    "cmake.cmakePath": "C:/msys64/mingw64/bin/cmake",
    "cmake.debugConfig": {
        "miDebuggerPath": "C:/msys64/mingw64/bin/gdb.exe",
        "externalConsole": true,
        "stopAtEntry": true,
        "setupCommands": [
            {
                "description": "Enable pretty-printing for gdb",
                "text": "-enable-pretty-printing",
                "ignoreFailures": true
            }
        ],
        "args": [
            "10"
        ]
    }
}
```

### 使用`MSYS Makefiles`的情况
> 需要指定`make`的路径`CMAKE_MAKE_PROGRAM`，此`make`通过在`MSYS2 MSYS`环境中使用`pacman -S msys/make`安装，`MSYS2 UCRT64`、`MSYS2 MINGW64`和`MSYS2 CLANG64`安装C++工具链时提供的`make`程序都是`mingw32-make.exe`，`mingw32-make.exe`适合使用`MinGW Makefiles`，一些教程会让你直接将`mingw32-make.exe`重命名为`make.exe`，不是那么合适
```
{
    "cmake.cmakePath": "C:/msys64/mingw64/bin/cmake",
    "cmake.generator": "MSYS Makefiles",
    "cmake.configureSettings": {
        "CMAKE_MAKE_PROGRAM": "C:/msys64/usr/bin/make.exe"
    },
    "cmake.debugConfig": {
        "miDebuggerPath": "C:/msys64/mingw64/bin/gdb.exe",
        "externalConsole": true,
        "stopAtEntry": true,
        "setupCommands": [
            {
                "description": "Enable pretty-printing for gdb",
                "text": "-enable-pretty-printing",
                "ignoreFailures": true
            }
        ],
        "args": [
            "10"
        ]
    }
}
```

### 注意 (不同环境同时添加到系统环境变量中很容易产生同名dll的不同版本冲突)
`strawberry-perl`的`perl\bin`如果也配置在环境变量中，它的`libstdc++-6.dll`可能与msys2对应环境中的`libstdc++-6.dll`的冲突，即vscode debug时使用了`perl`提供的`libstdc++-6.dll`而导致`std::cout`之类的函数遇到错误