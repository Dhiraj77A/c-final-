#include "../header_files/chessBoard.h"
#include "../header_files/Pawn.h"
#include "../header_files/Rook.h"
#include "../header_files/Knight.h"
#include "../header_files/Bishop.h"
#include "../header_files/Queen.h"
#include "../header_files/King.h"

#include <iostream>
#include <vector>
#include <limits>

chessBoard::chessBoard() : currentTurn(Color::WHITE)
{
    initializeBoard();
}
void chessBoard::initializeBoard()
{

    // inorder to initialize board first we need to clear the board
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            board[i][j] = nullptr;
        }
    }

    // after clearing the board we will start initializing pawn
    // pawn covers entire row which is 1 index row for black and 6 index row for white

    for (int j = 0; j < 8; j++)
    {
        board[1][j] = std::make_unique<pawn>(Color::BLACK);
        board[6][j] = std::make_unique<pawn>(Color::WHITE);
    }

    // rook cover edge of the board black at (0,0) and (0,7)
    board[0][0] = std::make_unique<rook>(Color::BLACK);
    board[0][7] = std::make_unique<rook>(Color::BLACK);

    // rook cover edge of the board white at (7,0) and (7,7)
    board[7][0] = std::make_unique<rook>(Color::WHITE);
    board[7][7] = std::make_unique<rook>(Color::WHITE);

    // at just the side of the rook there is knight
    // for black at position(0,1) and (0,6)
    board[0][1] = std::make_unique<knight>(Color::BLACK);
    board[0][6] = std::make_unique<knight>(Color::BLACK);

    // for white at position(7,1) and (7,6)
    board[7][1] = std::make_unique<knight>(Color::WHITE);
    board[7][6] = std::make_unique<knight>(Color::WHITE);

    // at just the side of the knight there is bishop
    // for black at position(0,2) and (0,5)
    board[0][2] = std::make_unique<bishop>(Color::BLACK);
    board[0][5] = std::make_unique<bishop>(Color::BLACK);

    // for black at position(7,2) and (7,5)
    board[7][2] = std::make_unique<bishop>(Color::WHITE);
    board[7][5] = std::make_unique<bishop>(Color::WHITE);

    // at the right side of 3rd column bishop is queen
    board[0][3] = std::make_unique<queen>(Color::BLACK);
    board[7][3] = std::make_unique<queen>(Color::WHITE);

    // at the left side of 5th column bishop is king
    board[0][4] = std::make_unique<king>(Color::BLACK);
    board[7][4] = std::make_unique<king>(Color::WHITE);
}


std::vector<position> chessBoard::getAttackableRoute(position attacker, position king)
{
    std::vector<position> route;

    // for straight line movement (Rooks and Queens)
    if (attacker.row == king.row || attacker.column == king.column)
    {
        int xStep = (king.row > attacker.row) ? 1 : (king.row < attacker.row) ? -1
                                                                              : 0;
        int yStep = (king.column > attacker.column) ? 1 : (king.column < attacker.column) ? -1
                                                                                          : 0;

        int x = attacker.row + xStep;
        int y = attacker.column + yStep;

        while (x != king.row || y != king.column)
        {
            route.push_back({x, y});
            x += xStep;
            y += yStep;
        }
    }

    // for diagonal line movement (Bishop and Queens)
    else if (abs(attacker.row - king.row) == abs(attacker.column - king.column))
    {
        int xStep = (king.row > attacker.row) ? 1 : -1;
        int yStep = (king.column > attacker.column) ? 1 : -1;

        int x = attacker.row + xStep;
        int y = attacker.column + yStep;

        while (x != king.row || y != king.column)
        {
            route.push_back({x, y});
            x += xStep;
            y += yStep;
        }
    }
    return route;
}


bool chessBoard::movePiece(int startX, int startY, int endX, int endY)
{

    // check if it is checkmate
    if (!isMoveValid(startX, startY, endX, endY, currentTurn))
    {
        if (isKingInCheck(currentTurn) && !hasAnyValidMove(currentTurn))
        {
            checkMate = true;
        }
        return false;
    }

    Color opponentColor = (currentTurn == Color::WHITE) ? Color::BLACK : Color::WHITE;

    Piece *pieceToMove = board[startX][startY].get();

    // it handle king and queen side castling
    if (pieceToMove->getType() == pieceType::KING && abs(startY - endY) == 2 && startX == endX)
    {
        int rookStartY = (endY > startY) ? 7 : 0;
        // King-side: rook to f (endY-1). Queen-side: rook to d (endY+1).
        int rookEndY = (endY > startY) ? (endY - 1) : (endY + 1);

        // revalidate castling legality here as well
        if (!isCastlingValid(startX, startY, startX, rookStartY, currentTurn))
        {
            return false;
        }
        // ensure destination squares are empty 
        if (!isEmptySquare(endX, endY) || !isEmptySquare(startX, rookEndY))
        {
            return false;
        }

        board[endX][endY] = std::move(board[startX][startY]);
        board[endX][endY]->setHasBeenMoved(true);

        board[startX][rookEndY] = std::move(board[startX][rookStartY]);
        board[startX][rookEndY]->setHasBeenMoved(true);

        std::cout << (endY > startY ? "King-side" : "Queen-side") << " castling!!!" << std::endl;
    enPassantTargetColumn = -1;
    enPassantTargetRow = -1;
    std::cout << "Move is valid." << std::endl;
    currentTurn = opponentColor;
    return true;
    }

    // Special Move: en-Passant
    // it handle en-Passant move

    if (pieceToMove->getType() == pieceType::PAWN && endX == enPassantTargetRow && endY == enPassantTargetColumn)
    {
        board[endX][endY] = std::move(board[startX][startY]);

        int capturedPawnRow = (currentTurn == Color::WHITE) ? endX + 1 : endX - 1;
        board[capturedPawnRow][endY] = nullptr;

        board[endX][endY]->setHasBeenMoved(true);
        std::cout << "Special Move: en-Passant\n";

    enPassantTargetColumn = -1;
    enPassantTargetRow = -1;
    std::cout << "Move is valid." << std::endl;
    currentTurn = opponentColor;
    return true;
    }

    // capture Declaration Message (Format: Q7dxB4)

    if (board[endX][endY])
    {
        char attackingPiece = board[startX][startY]->getSymbol();
        char capturedPiece = board[endX][endY]->getSymbol();
        char startFile = 'a' + startY;
        int startRank = 8 - startX;
        char endFile = 'a' + endY;
        int endRank = 8 - endX;
        std::cout << attackingPiece << startRank << startFile << "x" << capturedPiece << endRank << endFile << "\n";
    }

    // normal Move of Piece
    board[endX][endY] = std::move(board[startX][startY]);
    board[endX][endY]->setHasBeenMoved(true);

    // double pawn move for en-Passant target square
    if (pieceToMove->getType() == pieceType::PAWN && abs(endX - startX) == 2)
    {
        enPassantTargetRow = (startX + endX) / 2;
        enPassantTargetColumn = startY;
    }
    else
    {
        enPassantTargetColumn = -1;
        enPassantTargetRow = -1;
    }

    // pawn promotion
    if (board[endX][endY]->getType() == pieceType::PAWN && (endX == 0 || endX == 7))
    {
        pawnPromotion(endX, endY);
    }

    // check game state like checks, checkmate
    if (isCheckmate(opponentColor))
    {
        std::cout << "CHECKMATE\n"
                  << (currentTurn == Color::WHITE ? "WHITE" : "BLACK") << " wins!" << std::endl;
        gameOver = true;
        checkMate = true;
    }
    else if (isKingInCheck(opponentColor))
    {
        std::cout << "CHECK\n";
    }
    std::cout << "Move is valid." << std::endl;
    currentTurn = opponentColor;
    return true;
}

// checks if the square is empty or not
bool chessBoard::isEmptySquare(int x, int y) const
{
    return board[x][y] == nullptr;
}

// to get the piece at the specified position
Piece *chessBoard::getPieceAt(int x, int y) const
{
    return board[x][y].get();
}

// gets the king position
position chessBoard::getKingPosition(Color kingColor) const
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (board[i][j])
            {
                if (board[i][j]->getColor() == kingColor && board[i][j]->getType() == pieceType::KING)
                {
                    return position{i, j};
                }
            }
        }
    }
    throw std::runtime_error("King not found");
}

// check it the enemy piece posses threat to attack the piece at (x,y)
bool chessBoard::canEnemyPieceAttack(int targetX, int targetY, Color enemyColor) const
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (board[i][j] && board[i][j]->getColor() == enemyColor && board[i][j]->isValidMove(i, j, targetX, targetY, *this))
            {
                return true;
            }
        }
    }
    return false;
}

// check if the king is in check or not
bool chessBoard::isKingInCheck(Color kingColor) const
{
    position kingPosition = getKingPosition(kingColor);
    return canEnemyPieceAttack(kingPosition.row, kingPosition.column, (kingColor == Color::WHITE) ? Color::BLACK : Color::WHITE);
}

// simulates move and checks for king exposure
// check for illegal move if piece is protecting the king and when it move it can expose king to attack
// then that become illegal
// it just simulate itself and reset the postion in logic
bool chessBoard::doesMovePutKingInCheck(int startX, int startY, int endX, int endY, Color playerColor)
{
    auto movingPiece = std::move(board[startX][startY]);
    auto capturedPiece = std::move(board[endX][endY]);

    board[endX][endY] = std::move(movingPiece);

    bool result = isKingInCheck(playerColor);

    board[startX][startY] = std::move(board[endX][endY]);
    board[endX][endY] = std::move(capturedPiece);
    return result;
}

bool chessBoard::isMoveValid(int startX, int startY, int endX, int endY, Color playerColor)
{
    // first validate board bounds
    if (startX < 0 || startX >= 8 || startY < 0 || startY >= 8 || 
        endX < 0 || endX >= 8 || endY < 0 || endY >= 8) {
        return false;
    }

    // check if there is a piece at the start position
    Piece *pieceToMove = board[startX][startY].get();
    if (!pieceToMove) {
        return false;
    }

    // check if the piece belongs to the current player
    if (pieceToMove->getColor() != playerColor) {
        return false;
    }


    if (!pieceToMove || pieceToMove->getColor() != playerColor)
    {
        return false;
    }

    // check if the castling is valid
    if (pieceToMove->getType() == pieceType::KING && abs(startY - endY) == 2 && startX == endX && !pieceToMove->getHasBeenMoved())
    {
        int rookColumn = (endY > startY) ? 7 : 0;
        // rook must exist and be unmoved
        if (!board[startX][rookColumn] || board[startX][rookColumn]->getType() != pieceType::ROOK || board[startX][rookColumn]->getHasBeenMoved())
        {
            return false;
        }
        return isCastlingValid(startX, startY, startX, rookColumn, playerColor);
    }

    // check if en-passant is valid
    if (pieceToMove->getType() == pieceType::PAWN && endX == enPassantTargetRow && endY == enPassantTargetColumn)
    {
        if (abs(endY - startY) == 1 && ((playerColor == Color::WHITE && endX == startX - 1) || (playerColor == Color::BLACK && endX == startX + 1)))
        {
            return true;
        }
    }

    // check if normal piece is valid
    if (!pieceToMove->isValidMove(startX, startY, endX, endY, *this))
    {
        return false;
    }

    if (pieceToMove->getType() != pieceType::KNIGHT && !isPathClear(startX, startY, endX, endY))
    {
        return false;
    }

    if (doesMovePutKingInCheck(startX, startY, endX, endY, playerColor))
    {
        return false;
    }
    
    // std::cout << "Move is valid." << std::endl;
    return true;
}

// handles checkmate

bool chessBoard::isCheckmate(Color color)
{
    if (!isKingInCheck(color))
    {
        return false;
    }
    position kingPosition = getKingPosition(color);
    Color enemyColor = (color == Color::WHITE) ? Color::BLACK : Color::WHITE;

    // check all the possible move of king first

    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            int x = kingPosition.row + i;
            int y = kingPosition.column + j;

            if (x >= 0 && x < 8 && y >= 0 && y < 8)
            {
                // don't check the square if we have our own piece
                if (board[x][y] && board[x][y]->getColor() == color)
                {
                    continue;
                }

                auto movingPiece = std::move(board[kingPosition.row][kingPosition.column]);
                auto capturedPiece = std::move(board[x][y]);

                board[x][y] = std::move(movingPiece);

                bool isKingStillInCheck = isKingInCheck(color);

                board[kingPosition.row][kingPosition.column] = std::move(board[x][y]);
                board[x][y] = std::move(capturedPiece);

                if (!isKingStillInCheck)
                {
                    return false;
                }
            }
        }
    }

    // check for all attacker second
    std::vector<position> attackers;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (board[i][j] && board[i][j]->getColor() == enemyColor)
            {
                if (board[i][j]->isValidMove(i, j, kingPosition.row, kingPosition.column, *this))
                {
                    attackers.push_back({i, j});
                }
            }
        }
    }

    // if there are multiple attacker then there is no any possible scenario where any piece could protect in that
    // case only king can move to save it
    if (attackers.size() > 1)
    {
        return true;
    }

    // if single attacker then piece can block it or even capture it
    position attacker = attackers[0];
    std::vector<position> path = getAttackableRoute(attacker, kingPosition);
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (board[i][j] && board[i][j]->getColor() == color)
            {
                // checking if piece can capture the attacker
                if (isMoveValid(i, j, attacker.row, attacker.column, color))
                {
                    return false;
                }

                // checking if piece can block the attacker
                for (auto &blockPostion : path)
                {
                    if (isMoveValid(i, j, blockPostion.row, blockPostion.column, color))
                    {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

bool chessBoard::hasAnyValidMove(Color color)
{
    // checking if king can move first
    position KingPosition = getKingPosition(color);
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            int x = KingPosition.row + i;
            int y = KingPosition.column + j;
            if (x >= 0 && x < 8 && y >= 0 && y < 8)
            {
                if (isMoveValid(KingPosition.row, KingPosition.column, x, y, color))
                {
                    return true;
                }
            }
        }
    }

    // then checking all other piece
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (board[i][j] && board[i][j]->getColor() == color)
            {

                // we will skip the king as we have checked above
                if (board[i][j]->getType() == pieceType::KING)
                {
                    continue;
                    ;
                }

                // checking all possible moves for this piece
                for (int k = 0; k < 8; k++)
                {
                    for (int l = 0; l < 8; l++)
                    {
                        if (isMoveValid(i, j, k, l, color))
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

// checking if game goes for stalemate i.e. draw
bool chessBoard::isStalemate(Color color)
{
    // king shall not be in check
    if (isKingInCheck(color))
    {
        return false;
    }
    // there shouldn't be any valid move
    if (hasAnyValidMove(color))
    {
        return false;
    }
    return true;
}

// checking if there is no hurdle for the castling
bool chessBoard::isCastlingPathOpen(int row, int startY, int endY) const
{
    int step = (endY > startY) ? 1 : -1;
    for (int j = startY + step; j != endY; j += step)
    {
        if (!isEmptySquare(row, j))
        {
            return false;
        }
    }
    return true;
}

// checking validity of castling
bool chessBoard::isCastlingValid(int kingXPos, int kingYPos, int rookXPos, int rookYPos, Color color)
{

    // king and rook must be in same row
    if (kingXPos != rookXPos)
    {
        return false;
    }
    if (!board[kingXPos][kingYPos] || board[kingXPos][kingYPos]->getType() != pieceType::KING)
    {
        return false;
    }
    if (!board[rookXPos][rookYPos] || board[rookXPos][rookYPos]->getType() != pieceType::ROOK)
    {
        return false;
    }

    // check if any of the piece have moved since game started
    if (board[kingXPos][kingYPos]->getHasBeenMoved() || board[rookXPos][rookYPos]->getHasBeenMoved())
    {
        return false;
    }

    // check path is clear
    if (!isCastlingPathOpen(kingXPos, kingYPos, rookYPos))
    {
        return false;
    }

    // check king is not in any form of check
    if (isKingInCheck(color))
    {
        return false;
    }

    // check king does not pass through any check in between the path
    int direction = (rookYPos > kingYPos) ? 1 : -1;
    for (int step = 1; step <= 2; ++step)
    {
        int col = kingYPos + step * direction;
        if (doesMovePutKingInCheck(kingXPos, kingYPos, kingXPos, col, color))
        {
            return false;
        }
    }
    return true;
}

// check if there is hurdle between starting pos and destination for the piece
// except knight as it can jump through pieces to reach in destination
bool chessBoard::isPathClear(int startX, int startY, int endX, int endY) const
{
    if (board[startX][startY]->getType() == pieceType::KNIGHT)
    {
        return true;
    }
    int xStep = 0, yStep = 0;

    // checking direction of movement
    if (endX != startX)
    {
        xStep = (endX > startX) ? 1 : -1;
    }
    if (endY != startY)
    {
        yStep = (endY > startY) ? 1 : -1;
    }

    int currentX = startX + xStep;
    int currentY = startY + yStep;

    // check each square along the path
    while (currentX != endX || currentY != endY)
    {
        if (!isEmptySquare(currentX, currentY))
        {
            return false;
        }
        currentX += xStep;
        currentY += yStep;
    }
    return true;
}

// for pawn promotion
void chessBoard::pawnPromotion(int x, int y)
{
    if (!board[x][y] || board[x][y]->getType() != pieceType::PAWN)
    {
        return;
    }

    Color color = board[x][y]->getColor();
    char choice;
    bool validChoice = false;

    // clearing any existing input
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "\n PAWN PROMOTION FOR " << (color == Color::WHITE ? "WHITE" : "BLACK") << "!\n";

    while (!validChoice)
    {
        std::cout << "Choose a piece to promote to (Q for Queen, R for Rook, B for Bishop, N for Knight): ";
        std::cin >> choice;
        choice = toupper(choice);

        switch (choice)
        {
        case 'Q':
            board[x][y] = std::make_unique<queen>(color);
            validChoice = true;
            break;
        case 'R':
            board[x][y] = std::make_unique<rook>(color);
            validChoice = true;
            break;
        case 'B':
            board[x][y] = std::make_unique<bishop>(color);
            validChoice = true;
            break;
        case 'N':
            board[x][y] = std::make_unique<knight>(color);
            validChoice = true;
            break;

        default:
            std::cout << "Invalid choice! Please enter Q, R, B, or N.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    std::cout << "Pawn promoted to " << board[x][y]->getSymbol() << "!\n";

    // clearing existing input
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

bool chessBoard::tryCastling(Color color, bool kingSide)
{
    int row = (color == Color::WHITE) ? 7 : 0;
    int kingY = 4;
    int rookY = kingSide ? 7 : 0;
    int targetKingY = kingSide ? 6 : 2;

    return movePiece(row, kingY, row, targetKingY);
}

void chessBoard::displayBoard() const
{
    std::cout << " a b c d e f g h\n";
    for (int i = 0; i < 8; i++)
    {
        std::cout << 8 - i << " ";
        for (int j = 0; j < 8; j++)
        {
            if (board[i][j])
            {
                std::cout << board[i][j]->getSymbol() << " ";
            }
            else
            {
                std::cout << ". ";
            }
        }
        std::cout << 8 - i << " ";
    }
    std::cout << " a b c d e f g h\n";

    std::cout << "\n"
              << (currentTurn == Color::WHITE ? "WHITE" : "BLACK") << " to move\n";
}