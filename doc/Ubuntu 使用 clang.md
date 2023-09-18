Ubuntu 下使用由 apt 安装的 clang 编译项目

### 安装clang
```
sudo apt install clang
```

### 可能会遇到的错误
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
