$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
$vswherePath = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
$msbuildPath = & $vswherePath -latest -products '*' -requires Microsoft.Component.MSBuild -find 'MSBuild\**\Bin\MSBuild.exe' | Select-Object -First 1
if (-not $msbuildPath) { throw 'MSBuild do Visual Studio não encontrado.' }
& $msbuildPath (Join-Path $projectRoot 'tests/Testes.vcxproj') /t:Build /p:Configuration=Debug /p:Platform=x64 /nologo /v:minimal
if ($LASTEXITCODE -ne 0) { throw 'Falha na compilação dos testes.' }
& (Join-Path $projectRoot 'tests/bin/Testes.exe')
if ($LASTEXITCODE -ne 0) { throw 'Um teste falhou.' }
