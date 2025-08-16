#pragma once
#include "Pieces.h"

class knight : public Piece {
    public:
    knight(Color c);
    bool isValidMove(int startX, int startY, int endX, int endY, const chessBoard& board) const override;
    char getSymbol() const override;
    pieceType getType() const override{
        return pieceType::KNIGHT;
    }

};