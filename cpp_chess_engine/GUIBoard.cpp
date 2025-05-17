#include "GUIBoard.hpp"
#include "ChessBoard.hpp"
#include "BitOps.hpp"

GUIBoard::GUIBoard(ChessBoard& cb) : cb(cb) {
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

void GUIBoard::syncBoardWithGUI() {

}

void GUIBoard::createSFMLWindow() {
    cb.syncBoardWithBitboards();
    
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

    // Highlight for attacked squares (semi-transparent red)
    sf::RectangleShape attackedHighlight({ float(cellSize), float(cellSize) });
    attackedHighlight.setFillColor({ 250,  50,  50, 150 });

    std::optional<sf::Vector2i> selectedSquare;
    std::optional<sf::Vector2i> attackSourceSquare;

    while (window.isOpen()) {
        // --- Event Handling ---
        while (const std::optional<sf::Event> ev = window.pollEvent()) {
            if (!ev.has_value()) break;
            const sf::Event& event = *ev;

            if (event.is<sf::Event::Closed>()) {
                window.close();
            }
            else if (auto mb = event.getIf<sf::Event::MouseButtonPressed>()) {
                int col = mb->position.x / cellSize;
                int row = mb->position.y / cellSize;

                if (mb->button == sf::Mouse::Button::Left) {
                    // clear any attack highlights on a normal (left) click
                    attackSourceSquare.reset();

                    if (col >= 0 && col < 8 && row >= 0 && row < 8) {
                        if (!selectedSquare) {
                            // select piece if one exists
                            if (getBoard()[row][col] != '.') {
                                selectedSquare = { col, row };
                            }
                        }
                        else {
                            // attempt move
                            int from_idx = (7 - selectedSquare->y) * 8 + selectedSquare->x;
                            int to_idx = (7 - row) * 8 + col;
                            if (cb.validateMove(from_idx, to_idx)) {
                                cb.movePiece(selectedSquare->y, selectedSquare->x, row, col);
                                cb.syncBoardWithBitboards();
                            }
                            selectedSquare.reset();
                        }
                    }
                }
                else if (mb->button == sf::Mouse::Button::Right) {
                    sf::Vector2i clicked{ col, row };
                    // toggle attack highlight on repeated right-click
                    if (attackSourceSquare && *attackSourceSquare == clicked) {
                        attackSourceSquare.reset();
                    }
                    else {
                        attackSourceSquare = clicked;
                    }
                    // also clear any normal selection
                    selectedSquare.reset();
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
            std::unordered_map<Bitboard, Bitboard> allMoves = cb.getAllMoves();
            int fromRow = selectedSquare.value().y;
            int fromCol = selectedSquare.value().x;

            // Calculate source bit index using the formula: (7 - row) * 8 + col.
            int from_idx = (7 - fromRow) * 8 + fromCol;
            Bitboard key = 1ULL << from_idx;

            // Retrieve the moves bitboard.
            Bitboard moves = allMoves[key];

            // Iterate through all bits in the `moves` bitboard.
            while (moves) {

                
                // Calculate the index of the destination square
                int to_idx = bitScanForward(moves);  // Count trailing zeros to get the index

                // Calculate the row and column from the bit index
                int toRow = 7 - (to_idx / 8);
                int toCol = to_idx % 8;

                moves &= moves - 1;


                // Highlight the valid move square
                validMoveHighlight.setPosition(sf::Vector2f(toCol * static_cast<float>(cellSize), toRow * static_cast<float>(cellSize)));
                window.draw(validMoveHighlight);

            }

            // Draw the selected square highlight on top
            selectedHighlight.setPosition(sf::Vector2f(fromCol * static_cast<float>(cellSize), fromRow * static_cast<float>(cellSize)));
            window.draw(selectedHighlight);
        }

        // attacked-square highlights
        if (attackSourceSquare.has_value()) {
			
            int fromRow = attackSourceSquare.value().y;
            int fromCol = attackSourceSquare.value().x;

            int from_idx = (7 - fromRow) * 8 + fromCol;

			Bitboard attacks = cb.getAttacks(from_idx);

            for (int r = 0; r < 8; ++r) {
                for (int c = 0; c < 8; ++c) {
                    //int target_idx = (7 - r) * 8 + c;
                    //if (cb.isAttacked(from_idx)) {
						int target_idx = cb.get_bitindex(r, c);
						if (attacks & (1ULL << target_idx)) {
							attackedHighlight.setPosition(sf::Vector2f(c * static_cast<float>(cellSize), r * static_cast<float>(cellSize)));
							window.draw(attackedHighlight);
						}
                        //attackedHighlight.setPosition(sf::Vector2f(c * static_cast<float>(cellSize), r * static_cast<float>(cellSize)));
                        //window.draw(attackedHighlight);
                    //}
                }
            }
            attackedHighlight.setPosition(sf::Vector2f(fromCol * static_cast<float>(cellSize), fromRow * static_cast<float>(cellSize)));
            window.draw(attackedHighlight);
        }

        // Draw the chess pieces.
        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                char piece = getBoard()[r][c];
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
