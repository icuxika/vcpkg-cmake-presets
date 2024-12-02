## 下载

```
.\GacUI-download.ps1
```

## 构建`Executables`

> 在`Developer PowerShell for VS 2022`中执行

```
MSBuild.exe .\Lib\Release\Tools\Executables\Executables.sln -p:Configuration=Release -p:Platform=x86
.\Lib\Release\Tools\CopyExecutables.ps1
```

## 生成

> `GacUI.xml` -> `<Exclude Pattern="Lib"/>`避免扫描`Lib`下的UI xml文件

```
.\Lib\Release\Tools\GacBuild.ps1 -FileName .\GacUI.xml
```