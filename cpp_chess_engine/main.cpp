#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <map>
#include <optional>
#include <cctype>
#include <SFML/Graphics/Sprite.hpp>

// Parse a FEN string to initialize the board array
void parseFEN(const std::string& fen, char board[8][8]) {
    // Fill board with empties initially
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c)
            board[r][c] = '.';
    }

    int row = 0, col = 0;
    for (char ch : fen) {
        if (ch == ' ') {
            break; // stop at end of piece placement section
        }
        else if (ch == '/') {
            // Move down to next rank (next row in our array)
            row++;
            col = 0;
        }
        else if (std::isdigit(ch)) {
            // Numeric character: skip that many empty squares
            int count = ch - '0';
            for (int i = 0; i < count; ++i) {
				if (col < 8) { //could remove for small increase in performance
                    board[row][col++] = '.';
                }
            }
        }
        else {
            // Piece character
            if (row < 8 && col < 8) {
                board[row][col] = ch;
            }
            col++;
        }
    }
}

// Print the board state to console (8x8 grid)
void printBoard(const char board[8][8]) {
    std::cout << "Board state:\n";
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            char symbol = board[r][c];
            std::cout << (symbol == '.' ? '.' : symbol);
            if (c < 7) std::cout << " ";
        }
        std::cout << "\n";
    }
}

class ChessBoard {
private:
    char board[8][8];  // 8x8 character array
    std::string fen;



    void parseFEN() {
        // Fill board with empties initially
        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c)
                board[r][c] = '.';
        }

        int row = 0, col = 0;
        for (char ch : fen) {
            if (ch == ' ') {
                break; // stop at end of piece placement section
            }
            else if (ch == '/') {
                // Move down to next rank (next row in our array)
                row++;
                col = 0;
            }
            else if (std::isdigit(ch)) {
                // Numeric character: skip that many empty squares
                int count = ch - '0';
                for (int i = 0; i < count; ++i) {
                    if (col < 8) { //could remove for small increase in performance
                        board[row][col++] = '.';
                    }
                }
            }
            else {
                // Piece character
                if (row < 8 && col < 8) {
                    board[row][col] = ch;
                }
                col++;
            }
        }
    }

public:
    // Constructor that takes a string
    ChessBoard(const std::string& str) : fen(str) {
        
        initialize();  // Additional setup function
    }

    // Function to perform additional initialization
    void initialize() {
        // Add any custom initialization logic here
        parseFEN();
        std::cout << "Initialization logic executed.\n";
    }

    // Setter function to update the string and re-parse
    void setString(const std::string& newFen) {
        fen = newFen;
        initialize();
    }

    // Getter function to return the board
    const char (*getBoard() const)[8] {
        return board;
    }


    void printBoard() const {
        std::cout << "Board state:\n";
        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                char symbol = board[r][c];
                std::cout << (symbol == '.' ? '.' : symbol);
                if (c < 7) std::cout << " ";
            }
            std::cout << "\n";
        }
    }

};

//number of cmd line args passed in, and the args themselves
int main(int argc, char** argv) {
	//begin code for initializing the fen string
    // Default to standard starting position FEN
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    if (argc > 1) {
        fen = argv[1]; // use provided FEN argument
    }
    else {
        // Optional: prompt user for a FEN string
        std::cout << "Enter FEN string (or press Enter for default): ";
        std::string input;
        std::getline(std::cin, input);
        if (!input.empty()) {
            fen = input;
        }
    }
	//end code for initializing the fen string
    ChessBoard cb(fen);
    cb.printBoard();
    std::cout << "Standard print board ++++++++++++++" << "\n";
    // Initialize the board array from the FEN string
    char board[8][8];
	parseFEN(fen, board); //parseFen function is void and sets board value accordingly to the fen string

    // Output the initial board state to the console
    printBoard(board);


    //begin gui stuff
    // Create the SFML window
    const int windowSize = 800;
    sf::RenderWindow window(sf::VideoMode({ windowSize, windowSize }), "Chess Board",
        sf::Style::Titlebar | sf::Style::Close); // creates a window to put visuals in

    int cellSize = windowSize / 8;  // size of each square in pixels

    // Load piece textures into a map for quick lookup
    std::map<char, sf::Texture> textures;
    std::string pieceTypes = "KQRBNP";  // piece letters for white
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

    // Prepare drawable shapes for board squares
    sf::RectangleShape square(sf::Vector2f((float)cellSize, (float)cellSize));
    sf::Color lightColor(240, 217, 181); // light squares (tan)
    sf::Color darkColor(181, 136, 99);   // dark squares (brown)

    // Highlight square for selected piece (semi-transparent green)
    sf::RectangleShape highlight(sf::Vector2f((float)cellSize, (float)cellSize));
    highlight.setFillColor(sf::Color(100, 250, 50, 150));

    std::optional<sf::Vector2i> selectedSquare; // currently selected square (if any)

    // Main application loop
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
                            char piece = board[from.y][from.x];
                            board[from.y][from.x] = '.';
                            board[row][col] = piece;
                            selectedSquare.reset(); // deselect after moving

                            printBoard(board); // update console output (optional)
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
            highlight.setPosition(sf::Vector2f(sel.x* static_cast<float>(cellSize), sel.y* static_cast<float>(cellSize)));
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

    return 0;
}
