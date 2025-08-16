#pragma once

enum class Color {WHITE, BLACK };
enum class pieceType {KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN};

class Piece{
    protected:
    Color color;
    bool hasBeenMoved=false;

    public:
    Piece(Color c) : color(c) {}
    virtual ~Piece() = default;
    virtual bool isValidMove(int startX, int startY, int endX, int endY, const class chessBoard& board) const = 0;
    virtual char getSymbol() const = 0;
    Color getColor() const { return color; }
    virtual pieceType getType() const = 0;
    bool getHasBeenMoved() const {
        return hasBeenMoved;
    }
    void setHasBeenMoved(bool moved) {
        hasBeenMoved = moved;
    }
};
