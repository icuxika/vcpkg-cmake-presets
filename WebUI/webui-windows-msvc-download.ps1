if (!(Test-Path -Path webui-lib)) {
    New-Item -ItemType Directory -Path webui-lib
}
Invoke-WebRequest -Uri "https://github.com/webui-dev/webui/releases/download/2.4.2/webui-windows-msvc-x64.zip" -OutFile ".\webui-lib\webui-windows-msvc-x64.zip"
Expand-Archive -LiteralPath ".\webui-lib\webui-windows-msvc-x64.zip" -DestinationPath ".\webui-lib"
Remove-Item -Path ".\webui-lib\webui-windows-msvc-x64.zip"