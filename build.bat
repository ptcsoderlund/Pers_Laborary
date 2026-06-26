@echo off
setlocal
rem Configure + build the pl library (Debug) into bin\debug.
rem
rem Uses Ninja: it's single-config and exports compile_commands.json (the
rem Visual Studio generator does not), which clangd needs. Clang is pinned as
rem the C compiler, so `clang` must be on PATH.
rem
rem Pass extra CMake args through, e.g. for a shared library:
rem   build.bat -DBUILD_SHARED_LIBS=ON

rem %~dp0 ends in a backslash; strip it so a quoted path with spaces doesn't
rem turn the closing quote into an escaped one (\").
set "ROOT=%~dp0"
if "%ROOT:~-1%"=="\" set "ROOT=%ROOT:~0,-1%"
set "BUILD_DIR=%ROOT%\bin\debug"

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

cmake -G Ninja -S "%ROOT%" -B "%BUILD_DIR%" -DCMAKE_C_COMPILER=clang -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON %*
if errorlevel 1 exit /b %errorlevel%

cmake --build "%BUILD_DIR%"
if errorlevel 1 exit /b %errorlevel%
