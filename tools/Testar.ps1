$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
$vswherePath = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
$msbuildPath = & $vswherePath -latest -products '*' -requires Microsoft.Component.MSBuild -find 'MSBuild\**\Bin\MSBuild.exe' | Select-Object -First 1
if (-not $msbuildPath) { throw 'MSBuild do Visual Studio não encontrado.' }
foreach ($name in @('Testes','ExperienceTests','AudioPcmTests')) {
  $projectPath = Join-Path $projectRoot "tests/$name.vcxproj"
  $gameSources = Join-Path $projectRoot 'BaseAppOGL/Scenes/UltimoSinal'
  & $msbuildPath $projectPath /t:Build /p:Configuration=Debug /p:Platform=x64 /nologo /v:minimal "/p:GameSources=$gameSources"
  if ($LASTEXITCODE -ne 0) { throw "Falha na compilação: $name" }
  & (Join-Path $projectRoot "tests/bin/$name.exe")
  if ($LASTEXITCODE -ne 0) { throw "Falha nos testes: $name" }
}
