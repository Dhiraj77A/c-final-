#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <string>

void launchGame(const std::string& folder, const std::string& exe) {
    std::system(("start \"\" /D \"" + folder + "\" \"" + exe + "\"").c_str());
}

int main() {
    sf::RenderWindow window(sf::VideoMode(640, 360), "CLASSICAL GAME");
    window.setVerticalSyncEnabled(true);

    // load button textures
    sf::Texture chessTexture, tttTexture;
    chessTexture.loadFromFile("logo/logo.png"); // chess button
    tttTexture.loadFromFile("logo/gar.png");    // tic-tac-toe button

    sf::Sprite chessButton(chessTexture);
    sf::Sprite tttButton(tttTexture);

    chessButton.setPosition(200.f, 120.f);
    tttButton.setPosition(350.f, 120.f);
    chessButton.setScale(0.2f, 0.2f);
    tttButton.setScale(0.2f, 0.2f);

    sf::Font font;
    font.loadFromFile("Tictactoe Project/arial.ttf");
    sf::Text title("CLASSICAL GAME", font, 40);
    title.setFillColor(sf::Color::Black);
    title.setPosition(180.f, 30.f);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2f mousePos((float)event.mouseButton.x, (float)event.mouseButton.y);
                if (chessButton.getGlobalBounds().contains(mousePos))
                    launchGame("Chess Project", "chess.exe");
                else if (tttButton.getGlobalBounds().contains(mousePos))
                    launchGame("Tictactoe Project", "Tictactoe.exe");
            }
        }

        window.clear(sf::Color(240, 240, 240));
        window.draw(title);
        window.draw(chessButton);
        window.draw(tttButton);
        window.display();
    }

    return 0;
}
