#requires -Version 5.1
param([ValidateRange(1, 32)][int]$Paralelismo = 4)
$ErrorActionPreference = 'Stop'
& (Join-Path $PSScriptRoot 'PrepararSDK.ps1') -Configuracao Debug -Paralelismo $Paralelismo
