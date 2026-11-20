@echo off
chcp 65001 >nul
cd /d "%~dp0"
g++ -std=c++17 -Wall -Wextra -pedantic -finput-charset=UTF-8 -fexec-charset=UTF-8 main.cpp menu_utils.cpp types_task.cpp -o Day2.exe
if errorlevel 1 (
 echo Compilation failed. Install MSYS2 UCRT64 g++ and add it to PATH.
 pause
 exit /b 1
)
echo Build successful: Day2.exe
pause
