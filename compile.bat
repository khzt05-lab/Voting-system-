@echo off
title C++ Voting System — Build
color 0A

echo.
echo  ============================================
echo   Building Voting System (C++17)...
echo  ============================================
echo.

g++ -std=c++17 -Wall -Wextra -O2 ^
    -I headers ^
    main.cpp ^
    src/Candidate.cpp ^
    src/Student.cpp ^
    src/Admin.cpp ^
    src/FileHandler.cpp ^
    src/VotingSystem.cpp ^
    -o VotingSystem.exe

IF %ERRORLEVEL% EQU 0 (
    echo.
    echo  [SUCCESS] Build complete!  ^>  VotingSystem.exe
    echo.
    echo  Running...
    echo  ============================================
    echo.
    VotingSystem.exe
) ELSE (
    echo.
    echo  [FAILED]  Build failed.  Check errors above.
    echo.
    pause
)
