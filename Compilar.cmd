@echo off
rem Compila Release e Debug, roda os testes e a verificacao de runtime.
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0tools\CompilarEVerificar.ps1"
if errorlevel 1 (
  echo.
  echo A compilacao ou a verificacao falhou. Leia as mensagens acima.
) else (
  echo.
  echo Tudo certo. Use Jogar.cmd para abrir o jogo.
)
pause
