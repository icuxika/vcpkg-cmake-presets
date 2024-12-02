if (!(Test-Path -Path Lib)) {
    New-Item -ItemType Directory -Path Lib
}
Invoke-WebRequest -Uri https://github.com/vczh-libraries/Release/archive/refs/tags/1.2.10.2.zip -OutFile .\Lib\1.2.10.2.zip
Expand-Archive -LiteralPath .\Lib\1.2.10.2.zip .\Lib\
Move-Item .\Lib\Release-1.2.10.2\ .\Lib\Release