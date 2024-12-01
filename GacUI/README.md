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

> 注意：当前会扫描不只`GacUI.xml`这个文件，还会扫描其他位置的UI文件，具体用法还需要确定

```
.\Lib\Release\Tools\GacBuild.ps1 -FileName .\GacUI.xml
```