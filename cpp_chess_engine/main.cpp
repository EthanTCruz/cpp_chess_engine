#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <map>
#include <optional>
#include <cctype>
#include <SFML/Graphics/Sprite.hpp>
#include <vector>
#include <bitset>
#include "ChessBoard.hpp"
#include "GUIBoard.hpp"




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

    const char (*board)[8] = cb.getBoard();

	GUIBoard gb(cb);
    gb.createSFMLWindow();


    return 0;
}
