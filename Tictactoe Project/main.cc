
#include <SFML/Graphics.hpp>
#include <array>
#include <iostream>
#include <string>

class Game {
private:
    sf::RenderWindow window{ sf::VideoMode(400, 400), "Tic-Tac-Toe" };
    sf::Font font;
    std::array<char, 9> board;
    bool isPlayerXTurn = true;

    bool gameWon = false;
    int winningPatternIndex = -1;
    bool isDraw = false;
    std::string resultMessage;

public:
    Game() {
        board.fill(' ');

        if (!font.loadFromFile("arial.ttf")) {
            std::cout << "Failed to load font!\n";
        }
    }

    void run() {
        while (window.isOpen()) {
            handleEvents();
            draw();
        }
    }

private:
    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (!gameWon && event.type == sf::Event::MouseButtonPressed)
                handleClick(event.mouseButton.x, event.mouseButton.y);
        }
    }

    void handleClick(int x, int y) {
        int row = y / 133;
        int col = x / 133;
        int index = row * 3 + col;

        if (board[index] == ' ') {
            board[index] = isPlayerXTurn ? 'X' : 'O';
            isPlayerXTurn = !isPlayerXTurn;

            char winner = checkWin();
            if (winner != ' ') {
                resultMessage = std::string(1, winner) + " won!";
                gameWon = true;
            }
            else if (checkDraw()) {
                resultMessage = "It's a draw!";
                isDraw = true;
            }
        }
    }

    void draw() {
        window.clear(sf::Color(220, 220, 220));

        drawGrid();
        drawMarks();

        if (gameWon)
            drawWinLine();

        if (gameWon || isDraw)
            drawResultText();

        window.display();
    }

    void drawGrid() {
        sf::RectangleShape line;
        line.setFillColor(sf::Color::Black);

        for (int i = 1; i < 3; i++) {
            // Horizontal
            line.setSize({ 400.f, 5.f });
            line.setPosition(0.f, i * 133.f);
            window.draw(line);

            // Vertical
            line.setSize({ 5.f, 400.f });
            line.setPosition(i * 133.f, 0.f);
            window.draw(line);
        }
    }

    void drawMarks() {
        sf::Text text("", font, 100);
        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                char mark = board[r * 3 + c];
                if (mark != ' ') {
                    text.setString(std::string(1, mark));
                    text.setFillColor(mark == 'X' ? sf::Color::Red : sf::Color::Blue);

                    sf::FloatRect bounds = text.getLocalBounds();
                    text.setOrigin(bounds.width / 2, bounds.height / 2 + bounds.top);
                    text.setPosition(c * 133 + 66.f, r * 133 + 66.f);
                    window.draw(text);
                }
            }
        }
    }

    char checkWin() {
        int winPatterns[8][3] = {
            {0,1,2},{3,4,5},{6,7,8}, // rows
            {0,3,6},{1,4,7},{2,5,8}, // columns
            {0,4,8},{2,4,6}          // diagonals
        };

        for (int i = 0; i < 8; i++) {
            int* p = winPatterns[i];
            if (board[p[0]] != ' ' &&
                board[p[0]] == board[p[1]] &&
                board[p[1]] == board[p[2]]) {
                winningPatternIndex = i;
                return board[p[0]];
            }
        }
        return ' ';
    }

    void drawWinLine() {
        if (winningPatternIndex == -1) return;

        sf::RectangleShape line;
        line.setFillColor(sf::Color::Black);
		line.setSize({ 380.f, 5.f }); // thickness = 5 and length = 380
		line.setOrigin(line.getSize().x / 2, line.getSize().y / 2); // Center the line

        switch (winningPatternIndex) {
        case 0: line.setPosition(200, 66);  line.setRotation(0); break;   // Row 1
        case 1: line.setPosition(200, 200); line.setRotation(0); break;   // Row 2
        case 2: line.setPosition(200, 333); line.setRotation(0); break;   // Row 3
        case 3: line.setPosition(66, 200);  line.setRotation(90); break;  // Col 1
        case 4: line.setPosition(200, 200); line.setRotation(90); break;  // Col 2
        case 5: line.setPosition(333, 200); line.setRotation(90); break;  // Col 3
        case 6: line.setPosition(200, 200); line.setRotation(45); break;  // Diagonal 1
        case 7: line.setPosition(200, 200); line.setRotation(-45); break; // Diagonal 2
        }

        window.draw(line);
    }
//display win, draw or lose

    void drawResultText() {
        if (resultMessage.empty()) return;

        sf::Text text(resultMessage, font, 36);
        text.setFillColor(sf::Color::Black);

        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.width / 2, bounds.height / 2 + bounds.top);
        text.setPosition(200.f, 200.f);

        
        sf::RectangleShape bg;
        bg.setSize({bounds.width + 40.f, bounds.height + 20.f});
        bg.setOrigin(bg.getSize().x / 2, bg.getSize().y / 2);
        bg.setPosition(200.f, 200.f);
        bg.setFillColor(sf::Color(255, 255, 255, 200));
        bg.setOutlineColor(sf::Color::Black);
        bg.setOutlineThickness(2.f);

        window.draw(bg);
        window.draw(text);
    }

	bool checkDraw() {
        for (char c : board) if (c == ' ') return false;
        return true;
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}
