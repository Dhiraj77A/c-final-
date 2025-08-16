#include "../header_files/chessBoard.h"
#include "../header_files/Rook.h"

rook::rook(Color c) : Piece(c) {}

bool rook::isValidMove(int startX, int startY, int endX, int endY, const chessBoard &board) const
{
    // rook shall move in straight line
    if (startX != endX && startY != endY)
    {
        return false;
    }

    // path shall be clear
    int stepX = (endX == startX) ? 0 : (endX > startX) ? 1
                                                       : -1;
    int stepY = (endY == startY) ? 0 : (endY > startY) ? 1
                                                       : -1;

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

char rook::getSymbol() const
{
    return (color == Color::WHITE) ? 'R' : 'r';
}