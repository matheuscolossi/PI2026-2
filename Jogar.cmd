@echo off
cd /d "%~dp0BaseAppOGL"
if exist "bin\InvestigacaoRelease\BaseAppOGL.exe" (
  "bin\InvestigacaoRelease\BaseAppOGL.exe"
) else (
  "bin\Debug\BaseAppOGL.exe"
)
if errorlevel 1 pause
