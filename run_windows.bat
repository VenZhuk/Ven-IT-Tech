@echo off
chcp 65001 >nul
cd /d "%~dp0"
if not exist Day2.exe call build_windows.bat
if exist Day2.exe Day2.exe
pause
