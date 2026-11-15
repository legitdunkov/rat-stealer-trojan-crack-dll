@echo off
cd /d "%~dp0\.."

g++ -std=c++17 -O2 -Wall -Wextra main.cpp -o program.exe
if errorlevel 1 exit /b 1

program.exe -optimal=dc -n=8 -count=3 -seed=1001
program.exe -optimal=lc -n=8 -count=3 -seed=2002
program.exe -optimal=deg -n=8 -count=3 -seed=3003

pause

