#pragma once
#include "Pieces.h"

class pawn : public Piece {
    public:
    pawn(Color c);
    bool isValidMove(int startX, int startY, int endX, int endY, const chessBoard& board) const override;
    char getSymbol() const override;
    pieceType getType() const override{
        return pieceType::PAWN;
    }
};