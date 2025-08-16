#pragma once
#include <array>
#include <vector>
#include <memory>
#include "Pieces.h"
#include "Rook.h"

struct position
{
    int row;
    int column;
};

class chessBoard
{
private:
    std::array<std::array<std::unique_ptr<Piece>, 8>, 8> board;
    bool gameOver = false;
    bool checkMate = false;
    Color currentTurn;
    bool isCastlingPathOpen(int row, int startY, int endY) const;
    bool isCastlingValid(int kingXPos, int kingYPos, int rookXPos, int rookYPos, Color color);
    bool isPathClear(int startX, int startY, int endX, int endY) const;
    void pawnPromotion(int x, int y);

public:
    chessBoard();
    bool isGameOver() const
    {
        return gameOver;
    }

    Color getPlayerTurn() const
    {
        return currentTurn;
    }

    void initializeBoard();

    bool movePiece(int startX, int startY, int endX, int endY);
    bool isEmptySquare(int x, int y) const;

    Piece *getPieceAt(int x, int y) const;
    void displayBoard() const;
    position getKingPosition(Color kingColor) const;
    bool canEnemyPieceAttack(int targetX, int targetY, Color enemyColor) const;
    bool isKingInCheck(Color kingColor) const;
    bool doesMovePutKingInCheck(int startX, int startY, int endX, int endY, Color playerColor);
    bool isMoveValid(int startX, int startY, int endX, int endY, Color playerColor);

    std::vector<position> getAttackableRoute(position attacker, position king);
    bool hasAnyValidMove(Color color);
    bool isCheckmate(Color color);
    bool isStalemate(Color color);
    bool tryCastling(Color color, bool kingSide);

    int enPassantTargetRow = -1;
    int enPassantTargetColumn = -1;
};