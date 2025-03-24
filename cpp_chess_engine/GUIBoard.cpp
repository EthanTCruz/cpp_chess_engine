#include "GUIBoard.hpp"
#include "ChessBoard.hpp"

GUIBoard::GUIBoard(ChessBoard& cb) : cb(cb), board(cb.getBoard()) {
    initialize();  // Additional setup function
}

void GUIBoard::initialize() {
    initializeTextures();
    std::cout << "Initialization logic executed.\n";
}

void GUIBoard::initializeTextures() {
    std::string pieceTypes = "KQRBNP";
    for (char p : pieceTypes) {
        std::string whiteImage = "piece_pngs/" + std::string(1, p) + "W.png";
        std::string blackImage = "piece_pngs/" + std::string(1, p) + "B.png";
        sf::Texture whiteTex, blackTex;
        if (!whiteTex.loadFromFile(whiteImage)) {
            std::cerr << "Could not load " << whiteImage << "\n";
        }
        if (!blackTex.loadFromFile(blackImage)) {
            std::cerr << "Could not load " << blackImage << "\n";
        }
        textures[p] = std::move(whiteTex);         // e.g. 'K' -> white king texture
        textures[std::tolower(p)] = std::move(blackTex); // e.g. 'k' -> black king texture
    }
}

void GUIBoard::createSFMLWindow() {
    const int windowSize = 800;
    sf::RenderWindow window(sf::VideoMode({ windowSize, windowSize }), "Chess Board",
        sf::Style::Titlebar | sf::Style::Close); // creates a window to put visuals in

    int cellSize = windowSize / 8;  // size of each square in pixels

    sf::RectangleShape square(sf::Vector2f((float)cellSize, (float)cellSize));
    sf::Color lightColor(240, 217, 181); // light squares (tan)
    sf::Color darkColor(181, 136, 99);   // dark squares (brown)

    // Highlight square for selected piece (semi-transparent green)
    sf::RectangleShape highlight(sf::Vector2f((float)cellSize, (float)cellSize));
    highlight.setFillColor(sf::Color(100, 250, 50, 150));

    std::optional<sf::Vector2i> selectedSquare; // currently selected square (if any)

    int fromRow;
    int fromCol;
    while (window.isOpen()) {
        // Event handling (SFML 3.0 style)
        while (const std::optional<sf::Event> ev = window.pollEvent()) {
            if (!ev.has_value()) break;
            const sf::Event& event = *ev;
            if (event.is<sf::Event::Closed>()) {
                window.close();
            }
            if (auto mb = event.getIf<sf::Event::MouseButtonPressed>()) {
                if (mb->button == sf::Mouse::Button::Left) {
                    // Map mouse click to board coordinates
                    int col = mb->position.x / cellSize;
                    int row = mb->position.y / cellSize;
                    if (col >= 0 && col < 8 && row >= 0 && row < 8) {
                        if (!selectedSquare.has_value()) {
                            // Select a piece if one is on the clicked square
                            if (board[row][col] != '.') {
                                selectedSquare = sf::Vector2i(col, row);
                            }
                        }
                        else {
                            // Move the previously selected piece to the new square
                            sf::Vector2i from = selectedSquare.value();
                            fromRow = from.y;
                            fromCol = from.x;
                            cb.movePiece(fromRow, fromCol, row, col);

                            selectedSquare.reset(); // deselect after moving
                            board = cb.getBoard();

                        }
                    }
                }
            }
        }


        // Rendering the board and pieces
        window.clear();
        // Draw squares
        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                square.setPosition(sf::Vector2f(c * static_cast<float>(cellSize), r * static_cast<float>(cellSize)));
                square.setFillColor(((r + c) % 2 == 0) ? lightColor : darkColor);
                window.draw(square);
            }
        }
        // Draw highlight for selected square
        if (selectedSquare.has_value()) {
            sf::Vector2i sel = selectedSquare.value();
            highlight.setPosition(sf::Vector2f(sel.x * static_cast<float>(cellSize), sel.y * static_cast<float>(cellSize)));
            window.draw(highlight);
        }
        // Draw pieces
        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                char piece = board[r][c];
                if (piece == '.') continue;
                auto texIt = textures.find(piece);
                if (texIt == textures.end()) continue; // texture not found (should not happen)
                sf::Sprite sprite(texIt->second);

                // Scale piece image to fit the square if needed
                sf::Vector2u texSize = texIt->second.getSize();
                sprite.setScale((sf::Vector2f((float)cellSize / texSize.x, (float)cellSize / texSize.y)));
                sprite.setPosition(sf::Vector2f(c * static_cast<float>(cellSize), r * static_cast<float>(cellSize)));
                window.draw(sprite);
            }
        }
        window.display();
    }
}
