param(
    [Parameter(Mandatory = $true)]
    [string]$Origem
)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
$sdkSource = (Resolve-Path -LiteralPath $Origem).Path.TrimEnd('\', '/')
$sdkTarget = [IO.Path]::GetFullPath((Join-Path $projectRoot 'OpenGL_SDK')).TrimEnd('\', '/')
$required = @(
    'glew/include/GL/glew.h',
    'glfw/include/GLFW/glfw3.h',
    'assimp/include/assimp/Importer.hpp',
    'freetype/include/ft2build.h',
    'glm/glm/glm.hpp'
)
foreach ($relative in $required) {
    if (-not (Test-Path -LiteralPath (Join-Path $sdkSource $relative))) {
        throw "SDK incompleto: $relative nao encontrado em $sdkSource."
    }
}
if ($sdkSource -ieq $sdkTarget) {
    Write-Host "SDK ja esta no local correto: $sdkTarget"
    exit 0
}
if ($sdkTarget.StartsWith($sdkSource + '\', [StringComparison]::OrdinalIgnoreCase)) {
    throw 'A pasta de destino nao pode estar dentro da pasta de origem.'
}
New-Item -ItemType Directory -Path $sdkTarget -Force | Out-Null
foreach ($name in @('glew', 'glfw', 'assimp', 'freetype', 'glm', 'imgui', 'stb', 'glad')) {
    $sourceFolder = Join-Path $sdkSource $name
    if (Test-Path -LiteralPath $sourceFolder) {
        Copy-Item -LiteralPath $sourceFolder -Destination $sdkTarget -Recurse -Force
    }
}
Write-Host "SDK configurado em $sdkTarget (ignorado pelo Git)."
Write-Host 'Execute tools/PrepararSDKDebug.ps1 ou tools/PrepararSDKRelease.ps1 para preparar a configuracao desejada.'
