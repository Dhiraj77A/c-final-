#pragma once
#include "Pieces.h"

class king : public Piece {
    public:
    king(Color c);
    bool isValidMove(int startX, int startY, int endX, int endY, const chessBoard& board) const override;
    char getSymbol() const override;
    bool hasBeenMoved=false;  //this is used for castling the king and rook
    pieceType getType() const override{
        return pieceType::KING;
    }
};