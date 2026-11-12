# Compila Release e Debug, roda os testes de logica e o --verify-game das duas
# configuracoes. Mensagens sem acentos: o Windows PowerShell 5.1 le scripts sem
# BOM como ANSI. Os logs ficam em logs/melhorias_2026-11-12.
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
$vswherePath = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
$msbuildPath = & $vswherePath -latest -products '*' -requires Microsoft.Component.MSBuild -find 'MSBuild\**\Bin\MSBuild.exe' | Select-Object -First 1
if (-not $msbuildPath) { throw 'MSBuild do Visual Studio nao encontrado.' }
$appFolder = Join-Path $projectRoot 'BaseAppOGL'
$solution = Join-Path $appFolder 'BaseAppOGL.sln'
$logs = Join-Path $projectRoot 'logs/melhorias_2026-11-12'
New-Item -ItemType Directory -Force -Path $logs | Out-Null

foreach ($configuration in @('Release', 'Debug')) {
  Write-Host "== Compilando $configuration x64 =="
  & $msbuildPath $solution /t:Build "/p:Configuration=$configuration" /p:Platform=x64 /nologo /v:minimal
  if ($LASTEXITCODE -ne 0) { throw "Falha na compilacao $configuration." }
}

Write-Host '== Testes de logica =='
& (Join-Path $PSScriptRoot 'Testar.ps1')

$outputs = @{ Release = 'bin/InvestigacaoRelease'; Debug = 'bin/Debug' }
foreach ($configuration in @('Release', 'Debug')) {
  Write-Host "== Verificacao de runtime ($configuration) =="
  $executable = Join-Path $appFolder (Join-Path $outputs[$configuration] 'BaseAppOGL.exe')
  $standardOutput = Join-Path $logs "verify_$configuration.log"
  $standardError = Join-Path $logs "verify_${configuration}_erros.log"
  $process = Start-Process -FilePath $executable -ArgumentList '--verify-game' -WorkingDirectory $appFolder -NoNewWindow -Wait -PassThru -RedirectStandardOutput $standardOutput -RedirectStandardError $standardError
  $summary = Select-String -Path $standardOutput -Pattern '^Runtime: ' | Select-Object -Last 1
  if ($summary) { Write-Host $summary.Line }
  $failed = @(Select-String -Path $standardOutput -Pattern '^FAIL ')
  foreach ($line in $failed) { Write-Host $line.Line }
  if ($process.ExitCode -ne 0 -or $failed.Count -gt 0) { throw "--verify-game falhou em $configuration (codigo $($process.ExitCode)). Veja $standardOutput" }
}
Write-Host ''
Write-Host 'Compilacao, testes de logica e verificacao de runtime concluidos sem falhas.'
Write-Host "Logs: $logs"
