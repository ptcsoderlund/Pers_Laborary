@echo off
setlocal

set "ROOT=%~dp0"
if "%ROOT:~-1%"=="\" set "ROOT=%ROOT:~0,-1%"
set "BUILD_DIR=%ROOT%\bin\debug"

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

cmake -G Ninja -S "%ROOT%" -B "%BUILD_DIR%" -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON %*
if errorlevel 1 exit /b %errorlevel%

cmake --build "%BUILD_DIR%" --target sandbox
if errorlevel 1 exit /b %errorlevel%

"%BUILD_DIR%\sandbox.exe"
