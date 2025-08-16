#include "../header_files/chessBoard.h"
#include "../header_files/King.h"
#include <cmath>
king::king(Color c) : Piece(c) {}

bool king::isValidMove(int startX, int startY, int endX, int endY, const chessBoard &board) const
{
    int dx = abs(endX - startX);
    int dy = abs(endY - startY);

    // king can move 1 square in any legal position
    if (dx <= 1 && dy <= 1)
    {
        return board.isEmptySquare(endX, endY) || (board.getPieceAt(endX, endY)->getColor() != color);
    }

    // castling is validated centrally in chessBoard::isMoveValid (isCastlingValid).
    return false;
}

char king::getSymbol() const
{
    return (color == Color::WHITE) ? 'K' : 'k';
}