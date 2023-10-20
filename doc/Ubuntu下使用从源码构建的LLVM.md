# Ubuntu 下使用源码构建的 LLVM 来编译项目

```
git clone --depth 1 https://github.com/llvm/llvm-project.git -b llvmorg-17.0.3
```

```
cmake -S llvm -B build -G Ninja -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;lldb;lld" -DCMAKE_INSTALL_PREFIX=/home/icuxika/SourceInstall/llvm -DCMAKE_BUILD_TYPE=Release -DLLVM_PARALLEL_COMPILE_JOBS=12 -DLLVM_PARALLEL_LINK_JOBS=4
```

```
cmake --build build
```

```
cmake --install build
```
