@echo off
REM Minimal build: just compile main.cc

g++ main.cc -o Tictactoe.exe -lsfml-graphics -lsfml-window -lsfml-system -std=c++17 -mwindows

echo Build finished.
