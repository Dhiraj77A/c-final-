@echo off
echo Building Chess Project...

g++ -std=c++17 -I "header_files" sourceCode\main.cc sourceCode\chessBoard.cc sourceCode\King.cc sourceCode\Knight.cc sourceCode\Bishop.cc sourceCode\Queen.cc sourceCode\Rook.cc sourceCode\Pawn.cc resources\appicon.o -o chess.exe -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -mwindows

if exist chess.exe (
    echo Build successful! chess.exe created.
) else (
    echo Build failed. Check error messages above.
)
