@echo off
title UIT Student Election System - Build

echo Building with C++17...
g++ -std=c++17 -Wall -Wextra -I headers main.cpp src/Admin.cpp src/Student.cpp src/FileHandler.cpp src/VotingSystem.cpp -o VotingSystem.exe

if %errorlevel% equ 0 (
    echo Build successful.
    echo Starting VotingSystem.exe...
    VotingSystem.exe
) else (
    echo Build failed. Read the compiler messages above.
    pause
)

