#requires -Version 5.1
param(
    [ValidateSet('Debug', 'Release')]
    [string]$Configuracao = 'Release',
    [ValidateRange(1, 32)]
    [int]$Paralelismo = 4
)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
$sdkRoot = Join-Path $projectRoot 'OpenGL_SDK'
$configurationFolder = $Configuracao.ToLowerInvariant()
$buildRoot = Join-Path $sdkRoot "$configurationFolder-build"
$installRoot = Join-Path $sdkRoot $configurationFolder
$logsRoot = Join-Path $projectRoot 'logs'
$vswherePath = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
if (-not (Test-Path -LiteralPath $vswherePath)) { throw 'Visual Studio Installer nao encontrado.' }
$vsRoot = & $vswherePath -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath | Select-Object -First 1
if (-not $vsRoot) { throw 'Instale Desenvolvimento para desktop com C++ no Visual Studio 2026.' }
$cmakePath = Join-Path $vsRoot 'Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe'
if (-not (Test-Path -LiteralPath $cmakePath)) { throw 'Instale Ferramentas CMake para C++ no Visual Studio.' }
New-Item -ItemType Directory -Path $buildRoot, $installRoot, $logsRoot -Force | Out-Null
$libraries = @(
    @{ Name = 'glfw'; Source = 'glfw'; Options = @('-DGLFW_BUILD_EXAMPLES=OFF', '-DGLFW_BUILD_TESTS=OFF', '-DGLFW_BUILD_DOCS=OFF', '-DBUILD_SHARED_LIBS=OFF') },
    @{ Name = 'glew'; Source = 'glew/build/cmake'; Options = @('-DBUILD_SHARED_LIBS=ON', '-DBUILD_UTILS=OFF') },
    @{ Name = 'freetype'; Source = 'freetype'; Options = @('-DBUILD_SHARED_LIBS=ON', '-DFT_DISABLE_ZLIB=ON', '-DFT_DISABLE_BZIP2=ON', '-DFT_DISABLE_PNG=ON', '-DFT_DISABLE_HARFBUZZ=ON', '-DFT_DISABLE_BROTLI=ON') },
    @{ Name = 'assimp'; Source = 'assimp'; Options = @('-DBUILD_SHARED_LIBS=ON', '-DASSIMP_BUILD_TESTS=OFF', '-DASSIMP_BUILD_ASSIMP_TOOLS=OFF', '-DASSIMP_BUILD_ZLIB=ON', '-DASSIMP_WARNINGS_AS_ERRORS=OFF', '-DASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT=OFF', '-DASSIMP_BUILD_3DS_IMPORTER=ON', '-DASSIMP_BUILD_OBJ_IMPORTER=ON', '-DASSIMP_BUILD_FBX_IMPORTER=ON', '-DASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT=OFF') }
)
function Invoke-SDKCMake {
    param([string[]]$CMakeArguments, [string]$LogPath)
    $previousPreference = $ErrorActionPreference
    $cmakeExitCode = $null
    try {
        # PowerShell 5.1 converts redirected native stderr into error records.
        # CMake warnings must not abort a successful native command.
        $ErrorActionPreference = 'Continue'
        $global:LASTEXITCODE = $null
        & $cmakePath @CMakeArguments 2>&1 | Out-File -LiteralPath $LogPath -Append -Encoding utf8
        $cmakeExitCode = $global:LASTEXITCODE
    } finally {
        $ErrorActionPreference = $previousPreference
    }
    if ($null -eq $cmakeExitCode -or $cmakeExitCode -ne 0) {
        Get-Content -LiteralPath $LogPath -Tail 60
        throw "CMake falhou. Consulte $LogPath."
    }
}
foreach ($library in $libraries) {
    $source = Join-Path $sdkRoot $library.Source
    if (-not (Test-Path -LiteralPath (Join-Path $source 'CMakeLists.txt'))) {
        throw "Fontes de $($library.Name) ausentes. Execute tools/ConfigurarSDK.ps1 com o SDK completo do professor."
    }
    $build = Join-Path $buildRoot $library.Name
    $log = Join-Path $logsRoot "$configurationFolder-sdk-$($library.Name).log"
    Set-Content -LiteralPath $log -Value "Preparando $($library.Name) $Configuracao x64" -Encoding utf8
    Write-Host "Preparando $($library.Name) $Configuracao x64..."
    $configure = @('-S', $source, '-B', $build, '-G', 'Visual Studio 18 2026', '-A', 'x64', "-DCMAKE_INSTALL_PREFIX=$installRoot", '-DCMAKE_POLICY_VERSION_MINIMUM=3.5') + $library.Options
    Invoke-SDKCMake -CMakeArguments $configure -LogPath $log
    Invoke-SDKCMake -CMakeArguments @('--build', $build, '--config', $Configuracao, '--parallel', "$Paralelismo") -LogPath $log
    Invoke-SDKCMake -CMakeArguments @('--install', $build, '--config', $Configuracao) -LogPath $log
    Write-Host "OK: $($library.Name)"
}
Write-Host "SDK $Configuracao preparado. Compile a solucao em $Configuracao | x64."
