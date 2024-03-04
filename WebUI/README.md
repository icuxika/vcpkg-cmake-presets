# WebUI C++ minimal

## 下载 WebUI 的头文件和依赖库
```
cd .\WebUI\
.\webui-windows-msvc-download.ps1
cd ..
```

## cl
### 编译
```
cmake --preset windows-default-user
cmake --build --preset windows-default-release-user --target webuiMinimal
```

### 运行
```
.\out\build\windows-default\WebUI\Release\webuiMinimal.exe
```

## clang-cl
### 编译
```
cmake --preset windows-clang-cl-user
cmake --build --preset windows-clang-cl-release-user --target webuiMinimal
```

### 运行
```
.\out\build\windows-clang-cl\WebUI\Release\webuiMinimal.exe
```


## 使用`cl.exe`在命令行编译的方式
```
cl.exe /EHsc /std:c++17 main.cpp /IC:\Users\icuxika\Downloads\webui-windows-msvc-x64\include\ /link /LIBPATH:C:\Users\icuxika\Downloads\webui-windows-msvc-x64\ /MACHINE:X64 /SUBSYSTEM:WINDOWS webui-2-static.lib user32.lib Advapi32.lib Shell32.lib /OUT:main.exe
```