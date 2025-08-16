@echo off

g++ -std=c++17 "resources\launcher.cc" "resources\icon.o" -O2 -o "ClassicalGame.exe" -lsfml-graphics -lsfml-window -lsfml-system -mwindows
