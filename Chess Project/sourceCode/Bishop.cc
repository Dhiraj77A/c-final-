#include "../header_files/chessBoard.h"
#include "../header_files/Bishop.h"
#include <cmath>

bishop::bishop(Color c) : Piece(c) {}

bool bishop::isValidMove(int startX, int startY, int endX, int endY, const chessBoard &board) const
{
    // bishop diagonally
    int dx = abs(endX - startX);
    int dy = abs(endY - startY);
    if (dx != dy)
    {
        return false;
    }

    // path clearance
    int stepX = (endX > startX) ? 1 : -1;
    int stepY = (endY > startY) ? 1 : -1;

    int x = startX + stepX;
    int y = startY + stepY;

    while (x != endX || y != endY)
    {
        if (!board.isEmptySquare(x, y))
        {
            return false;
        }
        x += stepX;
        y += stepY;
    }

    // validate destination
    return board.isEmptySquare(endX, endY) || (board.getPieceAt(endX, endY)->getColor() != color);
}

char bishop::getSymbol() const
{
    return (color == Color::WHITE) ? 'B' : 'b';
}