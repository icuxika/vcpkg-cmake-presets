# C Wrapper For C++ Library

## `run_extract.ps1`

调用`jextract`命令生成`Java`端代码使`Java`代码能够通过`Foreign Function and Memory (FFM) API`调用C++实现的库

### 示例

```shell
C:\Users\icuxika\VSCodeProjects\vcpkg-cmake-presets\WrapperLib2C\run_extract.ps1 -Output .\src\main\java\ -Package com.icuxika.jextract.demo2
```

在目录`C:\Users\icuxika\VSCodeProjects\JavaFX-Package-Sample`下执行上述示例，
那么将会在`C:\Users\icuxika\VSCodeProjects\JavaFX-Package-Sample\src\main\java`目录下，
创建包目录`com.icuxika.jextract.demo2`并在此包目录下生成相关`Java`代码

> `Output`输入的目录将基于调用此脚本的目录来组合出完成的输出目录

### `Java`端相关代码调用示例

```
try (Arena arena = Arena.ofConfined()) {
    MemorySegment instance = createLibDemo.makeInvoker().apply();
    setValue(instance, 10);
    System.out.println(getValue(instance));
    deleteLibDemo(instance);
}
```