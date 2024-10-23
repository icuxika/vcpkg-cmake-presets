if (!(Test-Path -Path libffmpeg)) {
    New-Item -ItemType Directory -Path libffmpeg
}
Invoke-WebRequest -Uri "https://github.com/icuxika/FFmpegWindowsBuild/releases/download/5.1.git/5.1.git_static_simple_msvc_x64_2024-10-23.zip" -OutFile ".\libffmpeg\libffmpeg_msvc17_x64.zip"
Expand-Archive -LiteralPath ".\libffmpeg\libffmpeg_msvc17_x64.zip" -DestinationPath ".\libffmpeg"
Remove-Item -Path ".\libffmpeg\libffmpeg_msvc17_x64.zip"