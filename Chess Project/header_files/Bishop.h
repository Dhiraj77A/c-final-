#pragma once
#include "Pieces.h"

class bishop : public Piece {
    public:
    bishop(Color c);
    bool isValidMove(int startX, int startY, int endX, int endY, const chessBoard& board) const override;
    char getSymbol() const override;
    pieceType getType() const override{
        return pieceType::BISHOP;
    }

};