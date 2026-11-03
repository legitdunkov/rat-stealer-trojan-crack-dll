@echo off
cd /d "%~dp0.."

if not exist program.exe (
    echo Error: program.exe not found
    echo Compile: g++ -std=c++17 -O2 main.cpp -o program.exe
    pause
    exit /b 1
)

echo [1] First MDS matrix
program.exe tests\mds_4x4.txt
echo.

echo [2] Second MDS matrix
program.exe tests\mds_4x4_second.txt
echo.

echo [3] Not MDS matrix
program.exe tests\not_mds_4x4.txt
echo.

echo [4] AES matrix
program.exe tests\aes_mds.txt
echo.

echo [5] Reducible polynomial
program.exe tests\reducible_poly.txt
echo.

echo [6] Wrong polynomial degree
program.exe tests\wrong_degree.txt
echo.

echo [7] Invalid matrix element
program.exe tests\invalid_element.txt
echo.

pause
