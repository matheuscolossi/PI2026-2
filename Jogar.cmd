@echo off
cd /d "%~dp0BaseAppOGL"
if not exist "bin\Debug\BaseAppOGL.exe" (
  echo Compile BaseAppOGL.sln em Debug x64 no Visual Studio primeiro.
  pause
  exit /b 1
)
start "O Ultimo Sinal" "bin\Debug\BaseAppOGL.exe"
