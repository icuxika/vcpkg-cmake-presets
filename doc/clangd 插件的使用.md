不删除 C/C++ Extension Pack 的情况下使用 clangd 插件（Windows环境）

## 用户配置（clangd插件会自动检测插件冲突并提示禁用C/C++ Extension Pack部分功能的禁用）
```
"cmake.buildDirectory": "${workspaceFolder}/out/build/windows-clang-cl", // 简单的CMake项目，此目录与clangd.arguments.--compile-commands-dir一致方便clangd插件读取compile_commands.json
"cmake.exportCompileCommandsFile": true,
"C_Cpp.intelliSenseEngine": "disabled",
"C_Cpp.formatting": "disabled",
"clangd.arguments": [
    "--all-scopes-completion", // 全局补全(补全建议会给出在当前作用域不可见的索引,插入后自动补充作用域标识符),例如在main()中直接写cout,即使没有`#include <iostream>`,也会给出`std::cout`的建议,配合"--header-insertion=iwyu",还可自动插入缺失的头文件
    "--background-index", // 后台分析并保存索引文件
    "--clang-tidy", // 启用 Clang-Tidy 以提供「静态检查」
    "--compile-commands-dir=${workspaceFolder}/out/build/clang-cl", // 编译数据库(compile_commands.json 文件)的目录位置,
    "--completion-parse=auto", // 当 clangd 准备就绪时，用它来分析建议
    "--function-arg-placeholders=true", // 补全函数时，将会给参数提供占位符，键入后按 Tab 可以切换到下一占位符，乃至函数末,
    "--header-insertion-decorators", // 输入建议中，已包含头文件的项与还未包含头文件的项会以圆点加以区分
    "--header-insertion=iwyu", // 插入建议时自动引入头文件 iwyu,
    "--log=verbose", // 让 Clangd 生成更详细的日志
    "--pretty", // 输出的 JSON 文件更美观,
    "-j=12", // 同时开启的任务数量
],
```

## cmake-presets的构建预设需要配置指定变量以支持生成`compile_commands.json`以使clangd生效
> 与用户配置中的`"cmake.exportCompileCommandsFile": true,`一同配置
```
"cacheVariables": {
    "CMAKE_EXPORT_COMPILE_COMMANDS": "1"
},
```
> Visual Studio默认提供的编译套件无法生成此文件，此项目使用随Visual Studio一起安装的`clang-cl`编译套件来完成，`也因此vscode需要从Visual Studio提供的命令行环境中启动，插件依赖的clangd等程序都已经提供`
```
> Get-Command clangd.exe

CommandType     Name                                               Version    Source
-----------     ----                                               -------    ------
Application     clangd.exe                                         0.0.0.0    C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\Llvm\x64\bin\clangd.exe

```

> 根据 cmake-presets 不同预设构建项目的目录不同，还需要主要注意`clangd.arguments.--compile-commands-dir`的配置更新，如果不想通过这个参数来指定`compile_commands.json`，也可以在项目根目录下创建一个软链接到构建目录下的文件
### Windows
> PowerShell 管理员模式
```
New-Item -ItemType SymbolicLink -Path compile_commands.json -Target .\out\build\windows-clang-cl\compile_commands.json
```
### Linux
```
ln -s out/build/windows-clang-cl/compile_commands.json .
```