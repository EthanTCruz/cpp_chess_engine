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

    sf::RectangleShape square(sf::Vector2f(static_cast<float>(cellSize), static_cast<float>(cellSize)));
    sf::Color lightColor(240, 217, 181); // light squares (tan)
    sf::Color darkColor(181, 136, 99);   // dark squares (brown)

    // Highlight for selected piece (semi-transparent green)
    sf::RectangleShape selectedHighlight(sf::Vector2f(static_cast<float>(cellSize), static_cast<float>(cellSize)));
    selectedHighlight.setFillColor(sf::Color(100, 250, 50, 150));

    // Highlight for valid moves (semi-transparent blue)
    sf::RectangleShape validMoveHighlight(sf::Vector2f(static_cast<float>(cellSize), static_cast<float>(cellSize)));
    validMoveHighlight.setFillColor(sf::Color(50, 100, 250, 150));

    // Holds the board coordinates of the currently selected square, if any.
    std::optional<sf::Vector2i> selectedSquare;

    while (window.isOpen()) {
        // --- Event Handling ---
        while (const std::optional<sf::Event> ev = window.pollEvent()) {
            if (!ev.has_value()) break;
            const sf::Event& event = *ev;
            if (event.is<sf::Event::Closed>()) {
                window.close();
            }
            if (auto mb = event.getIf<sf::Event::MouseButtonPressed>()) {
                if (mb->button == sf::Mouse::Button::Left) {
                    // Map mouse click to board coordinates.
                    int col = mb->position.x / cellSize;
                    int row = mb->position.y / cellSize;
                    if (col >= 0 && col < 8 && row >= 0 && row < 8) {
                        if (!selectedSquare.has_value()) {
                            // If no piece is currently selected, select one if there is a piece.
                            if (board[row][col] != '.') {
                                selectedSquare = sf::Vector2i(col, row);
                            }
                        }
                        else {
                            // A piece is already selected. Attempt to move to the clicked square.
                            // Compute bit indices for source and destination.
                            int from_idx = (7 - selectedSquare.value().y) * 8 + selectedSquare.value().x;
                            int to_idx = (7 - row) * 8 + col;
                            if (cb.validateMove(from_idx, to_idx)) {
                                // Only move if the validator returns true.
                                cb.movePiece(selectedSquare.value().y, selectedSquare.value().x, row, col);
                                board = cb.getBoard();
                            }
                            // Deselect after attempting the move.
                            selectedSquare.reset();
                        }
                    }
                }
            }
        }

        // --- Rendering ---
        window.clear();
        // Draw the board squares.
        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                square.setPosition(sf::Vector2f(c * static_cast<float>(cellSize), r * static_cast<float>(cellSize)));
                square.setFillColor(((r + c) % 2 == 0) ? lightColor : darkColor);
                window.draw(square);
            }
        }

        // If a piece is selected, check all 64 squares for valid moves and highlight them.
        if (selectedSquare.has_value()) {
            int fromRow = selectedSquare.value().y;
            int fromCol = selectedSquare.value().x;
            // Calculate source bit index using the formula: (7 - row) * 8 + col.
            int from_idx = (7 - fromRow) * 8 + fromCol;
            for (int r = 0; r < 8; ++r) {
                for (int c = 0; c < 8; ++c) {
                    // Skip the square that contains the selected piece.
                    if (r == fromRow && c == fromCol)
                        continue;
                    int to_idx = (7 - r) * 8 + c;
                    // Use ChessBoard's validate function to check move legality.
                    if (cb.validateMove(from_idx, to_idx)) {
                        validMoveHighlight.setPosition(sf::Vector2f(c * static_cast<float>(cellSize), r * static_cast<float>(cellSize)));
                        window.draw(validMoveHighlight);
                    }
                }
            }
            // Draw the selected square highlight on top.
            selectedHighlight.setPosition(sf::Vector2f(fromCol * static_cast<float>(cellSize), fromRow * static_cast<float>(cellSize)));
            window.draw(selectedHighlight);
        }

        // Draw the chess pieces.
        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                char piece = board[r][c];
                if (piece == '.') continue;
                auto texIt = textures.find(piece);
                if (texIt == textures.end()) continue; // texture not found (should not happen)
                sf::Sprite sprite(texIt->second);
                sf::Vector2u texSize = texIt->second.getSize();
                sprite.setScale(sf::Vector2f(static_cast<float>(cellSize) / texSize.x, static_cast<float>(cellSize) / texSize.y));
                sprite.setPosition(sf::Vector2f(c * static_cast<float>(cellSize), r * static_cast<float>(cellSize)));
                window.draw(sprite);
            }
        }
        window.display();
    }
}
