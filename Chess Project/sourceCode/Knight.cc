#include "../header_files/chessBoard.h"
#include "../header_files/Knight.h"
#include <cmath>

knight::knight(Color c) : Piece(c) {}

bool knight::isValidMove(int startX, int startY, int endX, int endY, const chessBoard &board) const
{
    // knight move l shape
    int dx = abs(endX - startX);
    int dy = abs(endY - startY);
    // If not L-shape, invalid
    if (!((dx == 2 && dy == 1) || (dx == 1 && dy == 2)))
    {
        return false;
    }

    // check destination
    // Knight can land on empty or capture opponent
    return board.isEmptySquare(endX, endY) || (board.getPieceAt(endX, endY)->getColor() != color);
}

char knight::getSymbol() const
{
    return (color == Color::WHITE) ? 'N' : 'n';
}