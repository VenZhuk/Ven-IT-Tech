@echo off
chcp 65001 >nul
cd /d "%~dp0"
g++ -std=c++20 -O2 -Wall -Wextra server.cpp -o server_day6.exe
if errorlevel 1 (echo Compilation failed & pause & exit /b 1)
echo Open http://127.0.0.1:18080
server_day6.exe .
pause
