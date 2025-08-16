#include "../header_files/chessBoard.h"
#include "../header_files/Pieces.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <memory>
#include <algorithm>
#include <vector>
#include <utility>
#include <string>
#include <cmath>

class Chess
{
private:
    sf::RenderWindow window;
    chessBoard board;
    sf::Texture pieceTexture[12];
    sf::Sprite pieceSprites[8][8];
    sf::RectangleShape squares[8][8];
    sf::Font font;
    sf::Text turnText;
    sf::Text statusText;
    // Move history
    std::vector<std::pair<std::string, std::string>> moveHistory;
    float historyPanelWidth = 260.f;
    float boardLeftPadding = 25.f;
    const float squareSizeConst = 80.0f;

    sf::RectangleShape restartCore;
    sf::CircleShape restartLeftCap;
    sf::CircleShape restartRightCap;
    sf::RectangleShape restartShadowCore;
    sf::CircleShape restartShadowLeft;
    sf::CircleShape restartShadowRight;
    sf::Text restartButtonText;
    bool restartHovered = false;
    float restartWidth = 200.f;
    float restartHeight = 48.f;
    sf::RectangleShape banner;
    sf::Text bannerText;
    // Sounds
    sf::SoundBuffer moveBuffer;
    sf::Sound moveSound;
    bool moveSoundLoaded = false;
    sf::SoundBuffer captureBuffer;
    sf::Sound captureSound;
    bool captureSoundLoaded = false;
    sf::SoundBuffer checkBuffer;
    sf::Sound checkSound;
    bool checkSoundLoaded = false;
    sf::SoundBuffer checkmateBuffer;
    sf::Sound checkmateSound;
    bool checkmateSoundLoaded = false;

    sf::SoundBuffer warmupBuffer;
    sf::Sound warmupSound;

    sf::RectangleShape overlayDim;
    sf::RectangleShape modalCore;
    sf::RectangleShape modalShadowCore;
    sf::CircleShape modalLeftCap;
    sf::CircleShape modalRightCap;
    sf::CircleShape modalShadowLeft;
    sf::CircleShape modalShadowRight;
    sf::Text modalTitle;
    sf::Text modalSubtitle;

    int selectedX = -1, selectedY = -1;
    bool gameOver = false;
    bool showBanner = false;

    sf::Color lightSquareColor = sf::Color(238, 238, 210);
    sf::Color darkSquareColor = sf::Color(118, 150, 86);
    sf::Color highlightColor = sf::Color(246, 246, 105, 180);
    sf::Color moveHintColor = sf::Color(106, 190, 109, 180);

    float getSquareSize() const { return squareSizeConst; }
    float getBoardStartX() const { return boardLeftPadding; }
    float getBoardStartY() const { return (window.getSize().y - getSquareSize() * 8) / 2.f; }

    std::string coordToNotation(int row, int col)
    {
        char file = 'a' + col;
        int rank = 8 - row;
        return std::string(1, file) + std::to_string(rank);
    }

    std::string pieceLetter(pieceType t)
    {
        switch (t)
        {
        case pieceType::KING:
            return "K";
        case pieceType::QUEEN:
            return "Q";
        case pieceType::ROOK:
            return "R";
        case pieceType::BISHOP:
            return "B";
        case pieceType::KNIGHT:
            return "N";
        default:
            return ""; // Pawn has no letter in SAN
        }
    }

    bool isCastlingMove(int fromX, int fromY, int toX, int toY, const Piece *mover)
    {
        if (!mover || mover->getType() != pieceType::KING)
            return false;
        return (fromX == toX) && (std::abs(toY - fromY) == 2);
    }

    // Determine if any other same-type piece can also move to (toX,toY) from a different square
    std::string disambiguator(pieceType t, Color color, int fromX, int fromY, int toX, int toY)
    {
        if (t == pieceType::PAWN)
            return "";
        bool sameFileExists = false;
        bool sameRankExists = false;
        for (int r = 0; r < 8; ++r)
        {
            for (int c = 0; c < 8; ++c)
            {
                if (r == fromX && c == fromY)
                    continue;
                Piece *p = board.getPieceAt(r, c);
                if (!p)
                    continue;
                if (p->getColor() != color)
                    continue;
                if (p->getType() != t)
                    continue;
                if (board.isMoveValid(r, c, toX, toY, color))
                {
                    if (c == fromY)
                        sameFileExists = true;
                    if (r == fromX)
                        sameRankExists = true;
                }
            }
        }
        std::string d;
        if (sameFileExists && sameRankExists)
        {
            d = std::string(1, char('a' + fromY)) + std::to_string(8 - fromX);
        }
        else if (sameFileExists)
        {
            d = std::to_string(8 - fromX); // rank
        }
        else if (sameRankExists)
        {
            d = std::string(1, char('a' + fromY)); // file
        }
        else
        {
            bool another = false;
            for (int r = 0; r < 8 && !another; ++r)
            {
                for (int c = 0; c < 8 && !another; ++c)
                {
                    if (r == fromX && c == fromY)
                        continue;
                    Piece *p = board.getPieceAt(r, c);
                    if (p && p->getColor() == color && p->getType() == t && board.isMoveValid(r, c, toX, toY, color))
                    {
                        another = true;
                    }
                }
            }
            if (another)
                d = std::string(1, char('a' + fromY));
        }
        return d;
    }

    std::string generateSAN(Color mover, int fromX, int fromY, int toX, int toY)
    {
        Piece *moverPiece = board.getPieceAt(fromX, fromY);
        if (!moverPiece)
            return coordToNotation(fromX, fromY) + "-" + coordToNotation(toX, toY);
        pieceType t = moverPiece->getType();

        // Castling
        if (isCastlingMove(fromX, fromY, toX, toY, moverPiece))
        {
            return (toY > fromY) ? "O-O" : "O-O-O";
        }

        // Capture detection: target occupied by enemy OR en passant
        bool wasTargetOccupied = false;
        Piece *target = board.getPieceAt(toX, toY);
        if (target && target->getColor() != mover)
            wasTargetOccupied = true;
        bool isEnPassant = false;
        if (t == pieceType::PAWN && !wasTargetOccupied)
        {
            if (board.enPassantTargetRow == toX && board.enPassantTargetColumn == toY)
            {
                isEnPassant = true;
            }
        }
        bool isCapture = wasTargetOccupied || isEnPassant;

        std::string san;
        if (t == pieceType::PAWN)
        {
            if (isCapture)
            {
                san += std::string(1, char('a' + fromY));
                san += 'x';
            }
            san += coordToNotation(toX, toY);
        }
        else
        {
            san += pieceLetter(t);
            san += disambiguator(t, mover, fromX, fromY, toX, toY);
            if (isCapture)
                san += 'x';
            san += coordToNotation(toX, toY);
        }

        // Promotion
        if (t == pieceType::PAWN)
        {
            bool toLastRank = (mover == Color::WHITE && toX == 0) || (mover == Color::BLACK && toX == 7);
            if (toLastRank)
                san += "=Q";
        }

        return san;
    }

    void addSanToHistory(Color mover, const std::string &san)
    {
        if (mover == Color::WHITE)
        {
            moveHistory.emplace_back(san, "");
        }
        else
        {
            if (moveHistory.empty())
                moveHistory.emplace_back("", san);
            else
                moveHistory.back().second = san;
        }
    }

    void loadTextures()
    {
        const std::string pieceNames[12] = {"W_king", "W_queen", "W_rook", "W_bishop", "W_knight", "W_pawn",
                                            "B_king", "B_queen", "B_rook", "B_bishop", "B_knight", "B_pawn"};

        for (int i = 0; i < 12; i++)
        {
            std::string filePath = "pieces_img/" + pieceNames[i] + ".png";
            if (!pieceTexture[i].loadFromFile(filePath))
            {
                std::cerr << "Error loading texture: " << filePath << std::endl;
            }
        }
    }

    void loadSounds()
    {
        // movement sound
        const char *candidates[] = {"pieces_img/move.ogg", "pieces_img/move.wav"};
        moveSoundLoaded = false;
        for (const char *path : candidates)
        {
            if (moveBuffer.loadFromFile(path))
            {
                moveSound.setBuffer(moveBuffer);
                moveSound.setVolume(75.f);
                moveSoundLoaded = true;
                break;
            }
        }

        // capture sound
        const char *captureCandidates[] = {"pieces_img/capture.ogg", "pieces_img/capture.wav"};
        captureSoundLoaded = false;
        for (const char *path : captureCandidates)
        {
            if (captureBuffer.loadFromFile(path))
            {
                captureSound.setBuffer(captureBuffer);
                captureSound.setVolume(80.f);
                captureSoundLoaded = true;
                break;
            }
        }
        // check sound
        const char *checkCandidates[] = {"pieces_img/check.ogg", "pieces_img/check.wav"};
        checkSoundLoaded = false;
        for (const char *path : checkCandidates)
        {
            if (checkBuffer.loadFromFile(path))
            {
                checkSound.setBuffer(checkBuffer);
                checkSound.setVolume(85.f);
                checkSoundLoaded = true;
                break;
            }
        }

        // Checkmate sound
        const char *checkmateCandidates[] = {"pieces_img/checkmate.ogg", "pieces_img/checkmate.wav"};
        checkmateSoundLoaded = false;
        for (const char *path : checkmateCandidates)
        {
            if (checkmateBuffer.loadFromFile(path))
            {
                checkmateSound.setBuffer(checkmateBuffer);
                checkmateSound.setVolume(90.f);
                checkmateSoundLoaded = true;
                break;
            }
        }

        {
            const unsigned int sampleRate = 44100;
            const unsigned int channelCount = 1;
            const unsigned int ms = 10;
            std::size_t sampleCount = (sampleRate * ms) / 1000;
            std::vector<sf::Int16> samples(sampleCount * channelCount, 0);
            if (warmupBuffer.loadFromSamples(samples.data(), static_cast<unsigned int>(samples.size()), channelCount, sampleRate))
            {
                warmupSound.setBuffer(warmupBuffer);
                warmupSound.setVolume(0.f);
                warmupSound.play();
            }
        }
    }

    inline void playInstantly(sf::Sound &s)
    {
        s.stop();
        s.setPlayingOffset(sf::Time::Zero);
        s.play();
    }

    void setupRestartButtonVisuals()
    {
        float w = restartWidth;
        float h = restartHeight;
        float radius = h / 2.f;
        float panelX = static_cast<float>(window.getSize().x) - (historyPanelWidth + 20.f);
        float panelY = 100.f;
        float reserved = h + 30.f;
        float panelHeight = static_cast<float>(window.getSize().y) - panelY - reserved;
        float x = panelX + (historyPanelWidth - w) / 2.f;
        float y = panelY + panelHeight + 15.f;

        sf::Vector2f coreSize(w - 2.f * radius, h);
        restartShadowCore.setSize(coreSize);
        restartShadowCore.setPosition(x + radius, y + 4.f);
        restartShadowCore.setFillColor(sf::Color(0, 0, 0, 80));

        restartShadowLeft = sf::CircleShape(radius, 40);
        restartShadowLeft.setPosition(x, y + 4.f);
        restartShadowLeft.setFillColor(sf::Color(0, 0, 0, 80));

        restartShadowRight = sf::CircleShape(radius, 40);
        restartShadowRight.setPosition(x + w - h, y + 4.f);
        restartShadowRight.setFillColor(sf::Color(0, 0, 0, 80));

        // Core pill (green)
        restartCore.setSize(coreSize);
        restartCore.setPosition(x + radius, y);
        sf::Color normalGreen(46, 204, 113);
        restartCore.setFillColor(normalGreen);

        restartLeftCap = sf::CircleShape(radius, 40);
        restartLeftCap.setPosition(x, y);
        restartLeftCap.setFillColor(normalGreen);

        restartRightCap = sf::CircleShape(radius, 40);
        restartRightCap.setPosition(x + w - h, y);
        restartRightCap.setFillColor(normalGreen);

        // Text
        restartButtonText.setFont(font);
        restartButtonText.setString("Restart");
        restartButtonText.setCharacterSize(22);
        restartButtonText.setFillColor(sf::Color::White);
        sf::FloatRect tb = restartButtonText.getLocalBounds();
        float tx = x + (w - tb.width) / 2.f - tb.left;
        float ty = y + (h - tb.height) / 2.f - tb.top - 2.f;
        restartButtonText.setPosition(tx, ty);
    }

    void updateRestartButtonHover(float mx, float my)
    {
        float w = restartWidth;
        float h = restartHeight;
        // compute same position as setup
        float panelX = static_cast<float>(window.getSize().x) - (historyPanelWidth + 20.f);
        float panelY = 100.f;
        float reserved = h + 30.f;
        float panelHeight = static_cast<float>(window.getSize().y) - panelY - reserved;
        float x = panelX + (historyPanelWidth - w) / 2.f;
        float y = panelY + panelHeight + 15.f;
        sf::FloatRect bounds(x, y, w, h);
        bool hovered = bounds.contains(mx, my);
        if (hovered == restartHovered)
            return;
        restartHovered = hovered;
        sf::Color color = restartHovered ? sf::Color(39, 174, 96) : sf::Color(46, 204, 113);
        restartCore.setFillColor(color);
        restartLeftCap.setFillColor(color);
        restartRightCap.setFillColor(color);
    }

    void initializeBoard()
    {
        const float squareSize = getSquareSize();
        const float startX = getBoardStartX();
        const float startY = getBoardStartY();

        // creating squares
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                squares[i][j].setSize(sf::Vector2f(squareSize, squareSize));
                squares[i][j].setPosition(startX + j * squareSize, startY + i * squareSize);
                squares[i][j].setFillColor((i + j) % 2 == 0 ? darkSquareColor : lightSquareColor);
            }
        }

        // initialize piece sprites
        updatePieceSprites();

        // load font
        if (!font.loadFromFile("pieces_img/arial.ttf"))
        {
            std::cerr << "Error loading font" << std::endl;
        }

        // setup turn text
        turnText.setFont(font);
        turnText.setCharacterSize(28); // Increased text size
        turnText.setFillColor(sf::Color::White);
        turnText.setPosition(25, 25); // Adjusted position
        updateTurnText();

        // status text
        statusText.setFont(font);
        statusText.setCharacterSize(24); // Increased text size
        statusText.setFillColor(sf::Color::White);
        statusText.setPosition(25, 60); // Adjusted position

        // restart button (pill)
        setupRestartButtonVisuals();

        overlayDim.setSize(sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));
        overlayDim.setPosition(0.f, 0.f);
        overlayDim.setFillColor(sf::Color(0, 0, 0, 140));

        modalTitle.setFont(font);
        modalTitle.setCharacterSize(44);
        modalTitle.setFillColor(sf::Color::White);
        modalSubtitle.setFont(font);
        modalSubtitle.setCharacterSize(26);
        modalSubtitle.setFillColor(sf::Color(220, 220, 220));
    }

    void setupGameOverModal(const std::string &title, const std::string &subtitle, sf::Color accent)
    {
        // Pill-style modal centered
        float w = 620.f;
        float h = 160.f;
        float radius = h / 2.f;
        float x = (window.getSize().x - w) / 2.f;
        float y = (window.getSize().y - h) / 2.f;

        // Shadow
        sf::Vector2f coreSize(w - 2.f * radius, h);
        modalShadowCore.setSize(coreSize);
        modalShadowCore.setPosition(x + radius, y + 6.f);
        modalShadowCore.setFillColor(sf::Color(0, 0, 0, 110));
        modalShadowLeft = sf::CircleShape(radius, 64);
        modalShadowRight = sf::CircleShape(radius, 64);
        modalShadowLeft.setPosition(x, y + 6.f);
        modalShadowRight.setPosition(x + w - h, y + 6.f);
        modalShadowLeft.setFillColor(sf::Color(0, 0, 0, 110));
        modalShadowRight.setFillColor(sf::Color(0, 0, 0, 110));

        // Core
        modalCore.setSize(coreSize);
        modalCore.setPosition(x + radius, y);
        modalCore.setFillColor(sf::Color(35, 39, 47, 235)); // dark slate
        modalLeftCap = sf::CircleShape(radius, 64);
        modalRightCap = sf::CircleShape(radius, 64);
        modalLeftCap.setPosition(x, y);
        modalRightCap.setPosition(x + w - h, y);
        modalLeftCap.setFillColor(sf::Color(35, 39, 47, 235));
        modalRightCap.setFillColor(sf::Color(35, 39, 47, 235));

        // Title with subtle shadow
        modalTitle.setString(title);
        sf::Text titleShadow = modalTitle;
        titleShadow.setFillColor(sf::Color(0, 0, 0, 150));
        // center
        sf::FloatRect tb = modalTitle.getLocalBounds();
        float titleX = x + (w - tb.width) / 2.f - tb.left;
        float titleY = y + 28.f - tb.top;
        titleShadow.setPosition(titleX + 2.f, titleY + 2.f);
        modalTitle.setPosition(titleX, titleY);

        // Subtitle
        modalSubtitle.setString(subtitle);
        sf::FloatRect sb = modalSubtitle.getLocalBounds();
        float subX = x + (w - sb.width) / 2.f - sb.left;
        float subY = y + 90.f - sb.top;
        modalSubtitle.setPosition(subX, subY);

        // Accent line
        banner.setSize(sf::Vector2f(140.f, 4.f));
        banner.setPosition(x + (w - 140.f) / 2.f, y + h - 28.f);
        banner.setFillColor(accent);
        banner.setOutlineThickness(0.f);
    }

    void updatePieceSprites()
    {
        const float squareSize = getSquareSize();
        const float startX = getBoardStartX();
        const float startY = getBoardStartY();

        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                Piece *piece = board.getPieceAt(i, j);
                if (piece)
                {
                    int textureIndex = static_cast<int>(piece->getType()) + (piece->getColor() == Color::WHITE ? 0 : 6);

                    pieceSprites[i][j].setTexture(pieceTexture[textureIndex]);
                    pieceSprites[i][j].setPosition(startX + j * squareSize, startY + i * squareSize);

                    // scale to fit the square
                    sf::FloatRect bounds = pieceSprites[i][j].getLocalBounds();
                    float scale = squareSize / std::max(bounds.width, bounds.height);
                    pieceSprites[i][j].setScale(scale, scale);

                    // center in square
                    pieceSprites[i][j].setPosition(startX + j * squareSize + (squareSize - bounds.width * scale) / 2,
                                                   startY + i * squareSize + (squareSize - bounds.height * scale) / 2);
                }
            }
        }
    }

    void updateTurnText()
    {
        turnText.setString("Current Turn: " + std::string(board.getPlayerTurn() == Color::WHITE ? "White" : "Black"));
    }

    void handleSquareClick(int x, int y)
    {
        if (gameOver)
        {
            return;
        }

        // convert window coordinate to board coordinate
        const float squareSize = getSquareSize();
        const float boardStartX = getBoardStartX();
        const float boardStartY = getBoardStartY();

        int boardX = (y - boardStartY) / squareSize;
        int boardY = (x - boardStartX) / squareSize;

        // check if click is outside the box
        if (boardX < 0 || boardX >= 8 || boardY < 0 || boardY >= 8)
        {
            selectedX = -1;
            selectedY = -1;
            return;
        }

        // if no piece is selected, select a piece of the current turn's color
        if (selectedX == -1)
        {
            Piece *piece = board.getPieceAt(boardX, boardY);
            if (piece && piece->getColor() == board.getPlayerTurn())
            {
                selectedX = boardX;
                selectedY = boardY;
            }
        }

        // if a piece is already selected try to move it
        else
        {

            Piece *moverPiece = board.getPieceAt(selectedX, selectedY);
            Color moverColor = moverPiece ? moverPiece->getColor() : board.getPlayerTurn();
            if (board.isMoveValid(selectedX, selectedY, boardX, boardY, moverColor))
            {

                std::string san = generateSAN(moverColor, selectedX, selectedY, boardX, boardY);
                // determine capture before the move (includes en passant)
                Piece *targetBefore = board.getPieceAt(boardX, boardY);
                bool wasDirectCapture = targetBefore && targetBefore->getColor() != moverColor;
                bool wasEnPassant = false;
                if (moverPiece && moverPiece->getType() == pieceType::PAWN && !wasDirectCapture)
                {
                    if (board.enPassantTargetRow == boardX && board.enPassantTargetColumn == boardY)
                    {
                        wasEnPassant = true;
                    }
                }
                if (board.movePiece(selectedX, selectedY, boardX, boardY))
                {
                    bool didCapture = wasDirectCapture || wasEnPassant;
                    if (didCapture)
                    {
                        if (captureSoundLoaded)
                            playInstantly(captureSound);
                        else if (moveSoundLoaded)
                            playInstantly(moveSound);
                    }
                    else
                    {
                        if (moveSoundLoaded)
                            playInstantly(moveSound);
                    }

                    if (board.isCheckmate(board.getPlayerTurn()))
                        san += '#';
                    else if (board.isKingInCheck(board.getPlayerTurn()))
                        san += '+';
                    if (!san.empty())
                    {
                        if (san.back() == '#')
                        {
                            if (checkmateSoundLoaded)
                                playInstantly(checkmateSound);
                            else if (checkSoundLoaded)
                                playInstantly(checkSound);
                        }
                        else if (san.back() == '+')
                        {
                            if (checkSoundLoaded)
                                playInstantly(checkSound);
                        }
                    }
                    addSanToHistory(moverColor, san);
                    updatePieceSprites();
                    updateTurnText();

                    // check for game over
                    if (board.isCheckmate(Color::WHITE) || board.isCheckmate(Color::BLACK))
                    {
                        gameOver = true;
                        showBanner = true;
                        std::string winner = (board.getPlayerTurn() == Color::WHITE ? "Black" : "White");
                        setupGameOverModal("Checkmate!", winner + " wins", sf::Color(231, 76, 60));
                    }
                    else if (board.isStalemate(board.getPlayerTurn()))
                    {
                        gameOver = true;
                        showBanner = true;
                        setupGameOverModal("Stalemate", "Game drawn", sf::Color(241, 196, 15));
                    }
                }
            }
            selectedX = -1;
            selectedY = -1;
        }
    }

    void restartGame()
    {
        board = chessBoard();
        selectedX = -1;
        selectedY = -1;
        gameOver = false;
        showBanner = false;
        moveHistory.clear();
        updatePieceSprites();
        updateTurnText();
    }

    void draw()
    {
        window.clear(sf::Color(50, 50, 50));

        // draw board squares
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                window.draw(squares[i][j]);
            }
        }

        // highlight selected squares
        if (selectedX != -1 && selectedY != -1)
        {
            sf::RectangleShape highlight = squares[selectedX][selectedY];
            highlight.setFillColor(highlightColor);
            window.draw(highlight);

            // show possible moves
            for (int i = 0; i < 8; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    if (board.isMoveValid(selectedX, selectedY, i, j, board.getPlayerTurn()))
                    {
                        sf::RectangleShape moveHint = squares[i][j];
                        moveHint.setFillColor(moveHintColor);
                        window.draw(moveHint);
                    }
                }
            }
        }

        // draw pieces
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                if (board.getPieceAt(i, j))
                {
                    window.draw(pieceSprites[i][j]);
                }
            }
        }

        // draw coordinates
        sf::Text coordinateText;
        coordinateText.setFont(font);
        coordinateText.setCharacterSize(16);
        coordinateText.setFillColor(sf::Color::White);

        const float squareSize = getSquareSize();
        const float boardStartX = getBoardStartX();
        const float boardStartY = getBoardStartY();

        // draw file letter
        for (int j = 0; j < 8; j++)
        {
            coordinateText.setString(std::string(1, 'a' + j));
            coordinateText.setPosition(boardStartX + j * squareSize + squareSize - 20, boardStartY + 8 * squareSize);
            window.draw(coordinateText);
        }

        // draw rank number
        for (int i = 0; i < 8; i++)
        {
            coordinateText.setString(std::to_string(8 - i));
            coordinateText.setPosition(boardStartX - 20, boardStartY + i * squareSize + 5);
            window.draw(coordinateText);
        }
        // draw ui elements
        window.draw(turnText);
        window.draw(statusText);
        // draw restart pill button
        window.draw(restartShadowLeft);
        window.draw(restartShadowRight);
        window.draw(restartShadowCore);
        window.draw(restartLeftCap);
        window.draw(restartRightCap);
        window.draw(restartCore);
        window.draw(restartButtonText);

        // history panel
        float panelX = window.getSize().x - historyPanelWidth - 20.f;
        float panelY = 100.f;
        float reserved = restartHeight + 30.f;
        sf::RectangleShape panelBg;
        panelBg.setPosition(panelX, panelY);
        panelBg.setSize(sf::Vector2f(historyPanelWidth, window.getSize().y - panelY - reserved));
        panelBg.setFillColor(sf::Color(20, 20, 20, 200));
        panelBg.setOutlineColor(sf::Color(255, 255, 255, 40));
        panelBg.setOutlineThickness(1.f);
        window.draw(panelBg);

        // Column headers
        sf::Text headWhite, headBlack;
        headWhite.setFont(font);
        headBlack.setFont(font);
        headWhite.setCharacterSize(18);
        headBlack.setCharacterSize(18);
        headWhite.setFillColor(sf::Color(200, 200, 200));
        headBlack.setFillColor(sf::Color(200, 200, 200));
        float whiteX = panelX + 16.f;
        float blackX = panelX + historyPanelWidth * 0.5f;
        headWhite.setString("White");
        headBlack.setString("Black");
        headWhite.setPosition(whiteX, panelY + 12.f);
        headBlack.setPosition(blackX, panelY + 12.f);
        window.draw(headWhite);
        window.draw(headBlack);

        // Draw move history
        float rowY = panelY + 40.f;
        sf::Text whiteText, blackText;
        whiteText.setFont(font);
        blackText.setFont(font);
        whiteText.setCharacterSize(20);
        blackText.setCharacterSize(20);
        whiteText.setFillColor(sf::Color(240, 240, 240));
        blackText.setFillColor(sf::Color(240, 240, 240));
        // positions already set above
        for (const auto &p : moveHistory)
        {
            if (rowY > panelBg.getPosition().y + panelBg.getSize().y - 24.f)
                break;
            whiteText.setString(p.first);
            blackText.setString(p.second);
            whiteText.setPosition(whiteX, rowY);
            blackText.setPosition(blackX, rowY);
            window.draw(whiteText);
            window.draw(blackText);
            rowY += 24.f;
        }

        // draw restart pill button under panel
        window.draw(restartShadowLeft);
        window.draw(restartShadowRight);
        window.draw(restartShadowCore);
        window.draw(restartLeftCap);
        window.draw(restartRightCap);
        window.draw(restartCore);
        window.draw(restartButtonText);

        // if game is over draw modal overlay
        if (showBanner)
        {
            window.draw(overlayDim);
            window.draw(modalShadowLeft);
            window.draw(modalShadowRight);
            window.draw(modalShadowCore);
            window.draw(modalLeftCap);
            window.draw(modalRightCap);
            window.draw(modalCore);
            sf::Text titleShadow = modalTitle;
            titleShadow.setFillColor(sf::Color(0, 0, 0, 150));
            titleShadow.move(2.f, 2.f);
            window.draw(titleShadow);
            window.draw(modalTitle);
            window.draw(modalSubtitle);
            window.draw(banner);
        }
        window.display();
    }

public:
    Chess() : window(sf::VideoMode(1080, 800), "Chess Game")
    {
        loadTextures();
        loadSounds();
        initializeBoard();
    }

    void run()
    {
        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                }
                else if (event.type == sf::Event::MouseMoved)
                {
                    updateRestartButtonHover(static_cast<float>(event.mouseMove.x), static_cast<float>(event.mouseMove.y));
                }
                else if (event.type == sf::Event::MouseButtonPressed)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        // check if restart pill is clicked
                        updateRestartButtonHover(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
                        if (restartHovered)
                        {
                            restartGame();
                            setupRestartButtonVisuals(); 
                        }
                        else
                        {
                            handleSquareClick(event.mouseButton.x, event.mouseButton.y);
                        }
                    }
                }
            }
            draw();
        }
    }
};

int main()
{
    Chess chess;
    chess.run();
    return 0;
}