param(
    [Parameter(Mandatory = $true)][string]$Output,
    [Parameter(Mandatory = $true)][string]$Package
)

$rootPath = Split-Path -Parent $PSScriptRoot
$buildPath = Join-Path -Path $rootPath  -ChildPath "out\build\windows-default\WrapperLib2C\Release"

$outputPath = (Resolve-Path -Path $Output).Path

$includePath = Join-Path -Path $PSScriptRoot -ChildPath "include"

$dllPath = Join-Path -Path $buildPath -ChildPath "lib-c-demo.dll"

$headerPath = Join-Path -Path $includePath -ChildPath "lib-demo-wrapper.h"

Write-Host "jextract:" -ForegroundColor Blue
Write-Host "    ps1: $PSScriptRoot" -ForegroundColor Blue
Write-Host "    output: $outputPath" -ForegroundColor Blue
Write-Host "    package: $Package" -ForegroundColor Blue
Write-Host "    include: $includePath" -ForegroundColor Blue
Write-Host "    dll: $dllPath" -ForegroundColor Blue
Write-Host "    header: $headerPath" -ForegroundColor Blue

try {
    $jextractPath = Get-Command jextract -ErrorAction Stop
    & $jextractPath --output $outputPath -t $Package -I $includePath -l $dllPath $headerPath
    Write-Host "jextract 已完成代码生成" -ForegroundColor Green
}
catch {
    throw "jextract 不存在于 PATH 中"
}
