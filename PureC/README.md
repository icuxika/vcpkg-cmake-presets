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