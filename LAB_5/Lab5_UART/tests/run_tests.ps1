param([string]$Compiler = "")

$ErrorActionPreference = "Stop"
if (-not $Compiler) {
    $gccCommand = Get-Command gcc -ErrorAction SilentlyContinue
    if ($gccCommand) { $Compiler = $gccCommand.Source }
    else {
        foreach ($candidate in @("C:/msys64/ucrt64/bin/gcc.exe", "C:/msys64/mingw64/bin/gcc.exe")) {
            if (Test-Path -LiteralPath $candidate) { $Compiler = $candidate; break }
        }
    }
}
if (-not $Compiler) { throw "Host GCC not found. Pass -Compiler with its full path." }

$projectDir = Split-Path $PSScriptRoot -Parent
$previousPath = $env:PATH
$env:PATH = (Split-Path $Compiler -Parent) + ';' + $env:PATH
Push-Location $projectDir
try {
    New-Item -ItemType Directory -Force -Path build | Out-Null
    & $Compiler -std=c11 -Wall -Wextra -Werror -pedantic -static `
        -Itests/include -ICore/Inc Core/Src/command_parser.c `
        Core/Src/uart_communication.c tests/test_protocol.c -o build/test_protocol.exe
    if ($LASTEXITCODE -ne 0) { throw "Host test compilation failed." }
    & ./build/test_protocol.exe
    if ($LASTEXITCODE -ne 0) { throw "Host protocol tests failed." }
}
finally {
    Pop-Location
    $env:PATH = $previousPath
}
