@echo off
cd /d "%~dp0BaseAppOGL"
if exist "bin\Release\BaseAppOGL.exe" (
  "bin\Release\BaseAppOGL.exe"
) else (
  "bin\Debug\BaseAppOGL.exe"
)
if errorlevel 1 pause
