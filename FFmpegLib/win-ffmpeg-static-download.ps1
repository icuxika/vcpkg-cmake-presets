if (!(Test-Path -Path libffmpeg)) {
    New-Item -ItemType Directory -Path libffmpeg
}
Invoke-WebRequest -Uri "https://github.com/ShiftMediaProject/FFmpeg/releases/download/6.1.r112164/libffmpeg_6.1.r112164_msvc17_x64.zip" -OutFile ".\libffmpeg\libffmpeg_msvc17_x64.zip"
Expand-Archive -LiteralPath ".\libffmpeg\libffmpeg_msvc17_x64.zip" -DestinationPath ".\libffmpeg"
Remove-Item -Path ".\libffmpeg\libffmpeg_msvc17_x64.zip"