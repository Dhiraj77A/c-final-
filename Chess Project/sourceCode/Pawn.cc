#include "../header_files/Pawn.h"
#include "../header_files/chessBoard.h"
#include <cmath>

pawn::pawn(Color c) : Piece(c) {}
bool pawn::isValidMove(int startX, int startY, int endX, int endY, const chessBoard &board) const
{
    int direction = (color == Color::WHITE) ? -1 : 1;

    // one or two step forward from beginning at first and then only single move if moved earlier
    if (startY == endY)
    {
        // single step
        if (endX == startX + direction && board.isEmptySquare(endX, endY))
        {
            return true;
        }

        // double step from beginning
        if (endX == startX + 2 * direction && board.isEmptySquare(endX, endY))
        {
            bool isStartingPostion = (color == Color::WHITE) ? (startX == 6) : (startX == 1);
            // Ensure the intermediate square is empty
            int midX = startX + direction;
            if (isStartingPostion && board.isEmptySquare(midX, endY))
            {
                return true;
            }
        }
    }
    else if (abs(endY - startY) == 1 && endX == startX + direction)
    {
        if (!board.isEmptySquare(endX, endY) && board.getPieceAt(endX, endY)->getColor() != color)
        {
            return true;
        }
    }
    return false;
}

char pawn::getSymbol() const
{
    return (color == Color::WHITE) ? 'P' : 'p';
}