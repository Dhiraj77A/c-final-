@echo off

g++ main.cc icon.o -o Tictactoe.exe -lsfml-graphics -lsfml-window -lsfml-system -std=c++17 -mwindows

