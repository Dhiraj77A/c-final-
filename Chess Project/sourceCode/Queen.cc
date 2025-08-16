#include "../header_files/chessBoard.h"
#include "../header_files/Queen.h"
#include <cmath>

queen::queen(Color c) : Piece(c) {}
bool queen::isValidMove(int startX, int startY, int endX, int endY, const chessBoard &board) const
{
    int dx = abs(endX - startX);
    int dy = abs(endY - startY);

    // queen shall move diagonally as well as straight
    if (dx != 0 && dy != 0 && dx != dy)
    {
        return false;
    }

    // checking if the path is clear
    int stepX = (endX > startX) ? 1 : (endX < startX) ? -1
                                                      : 0;
    int stepY = (endY > startY) ? 1 : (endY < startY) ? -1
                                                      : 0;

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

    // destination
    return board.isEmptySquare(endX, endY) || (board.getPieceAt(endX, endY)->getColor() != color);
}

char queen::getSymbol() const
{
    return (color == Color::WHITE) ? 'Q' : 'q';
}