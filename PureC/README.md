## `Ubuntu 22.04.5 LTS`使用`musl`静态编译c代码

### 编译 musl
```
git clone https://git.musl-libc.org/git/musl

cd musl/
./configure --prefix=/home/icuxika/CommandLineTools/musl --syslibdir=/home/icuxika/CommandLineTools/musl/lib --disable-shared

make
make install
```

### `CMakeUserPresets.json`
```
{
    "name": "musl",
    "inherits": "default",
    "generator": "Ninja Multi-Config",
    "binaryDir": "${sourceDir}/out/build/musl",
    "cacheVariables": {
        "CMAKE_C_FLAGS": "-nostdinc -I/home/icuxika/CommandLineTools/musl/include",
        "CMAKE_EXE_LINKER_FLAGS": "-L/home/icuxika/CommandLineTools/musl/lib -static -lc",
        "CMAKE_C_COMPILER": "musl-gcc",
        "CMAKE_INSTALL_PREFIX": "${sourceDir}/out/install/musl"
    },
    "environment": {
        "VCPKG_ROOT": "/home/icuxika/CommandLineTools/vcpkg"
    }
}
```

## `Ubuntu 22.04.5 LTS`使用`LLVM-libc`静态编译c代码

### `Overlay Mode`
#### 编译LLVM
```
cmake -S llvm -B build -G Ninja -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;lldb;lld" -DLLVM_ENABLE_RUNTIMES="libc;compiler-rt;libcxx;libcxxabi;libunwind" -DCMAKE_INSTALL_PREFIX=/home/icuxika/CommandLineTools/llvm -DCMAKE_BUILD_TYPE=Release -DLLVM_PARALLEL_COMPILE_JOBS=12 -DLLVM_PARALLEL_LINK_JOBS=4

cmake --build build
cmake --install build
```
#### `CMakeUserPresets.json`
```
{
    "name": "llvmlibc-overlay",
    "inherits": "default",
    "generator": "Ninja Multi-Config",
    "binaryDir": "${sourceDir}/out/build/llvmlibc-overlay",
    "cacheVariables": {
        "CMAKE_C_FLAGS": "",
        "CMAKE_EXE_LINKER_FLAGS": "-static -l:libllvmlibc.a",
        "CMAKE_C_COMPILER": "clang",
        "CMAKE_INSTALL_PREFIX": "${sourceDir}/out/install/llvmlibc-overlay"
    },
    "environment": {
        "VCPKG_ROOT": "/home/icuxika/CommandLineTools/vcpkg"
    }
}
```
### `Fullbuild Mode`
#### 编译LLVM（构建完全独立的`LLVM-libc`）
```
sudo apt install gcc-multilib
sudo apt install python3-pip
pip3 install -i https://repo.huaweicloud.com/repository/pypi/simple headergen
export PATH=/home/icuxika/.local/bin:$PATH
export PATH=/home/icuxika/CommandLineTools/llvm/bin:$PATH
```
```
SYSROOT=/home/icuxika/CommandLineTools/llvm-libc
cmake -S llvm -B build -G Ninja -DLLVM_ENABLE_PROJECTS="clang;lld;libc;compiler-rt;clang-tools-extra;lldb" -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DLLVM_LIBC_FULL_BUILD=ON -DLLVM_LIBC_INCLUDE_SCUDO=ON -DCOMPILER_RT_BUILD_SCUDO_STANDALONE_WITH_LLVM_LIBC=ON -DCOMPILER_RT_BUILD_GWP_ASAN=OFF -DCOMPILER_RT_SCUDO_STANDALONE_BUILD_SHARED=OFF -DCLANG_DEFAULT_LINKER=lld -DCLANG_DEFAULT_RTLIB=compiler-rt -DDEFAULT_SYSROOT=$SYSROOT -DCMAKE_INSTALL_PREFIX=$SYSROOT -DLLVM_PARALLEL_COMPILE_JOBS=12 -DLLVM_PARALLEL_LINK_JOBS=4
cmake --build build
cmake --install build
cd build/
ninja install-clang install-builtins install-compiler-rt install-core-resource-headers install-libc install-lld
```

#### `CMakeUserPresets.json`
```
{
    "name": "llvmlibc-fullbuild",
    "inherits": "default",
    "generator": "Ninja Multi-Config",
    "binaryDir": "${sourceDir}/out/build/llvmlibc-fullbuild",
    "cacheVariables": {
        "CMAKE_C_FLAGS": "",
        "CMAKE_EXE_LINKER_FLAGS": "-static -rtlib=compiler-rt -fuse-ld=lld",
        "CMAKE_C_COMPILER": "/home/icuxika/CommandLineTools/llvm-libc/bin/clang",
        "CMAKE_SYSROOT": "/home/icuxika/CommandLineTools/llvm-libc",
        "CMAKE_TRY_COMPILE_TARGET_TYPE": "STATIC_LIBRARY",
        "CMAKE_INSTALL_PREFIX": "${sourceDir}/out/install/llvmlibc-fullbuild"
    },
    "environment": {
        "VCPKG_ROOT": "/home/icuxika/CommandLineTools/vcpkg"
    }
}
```