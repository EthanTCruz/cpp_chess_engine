// GUIBoard.hpp
#ifndef GUIBOARD_HPP
#define GUIBOARD_HPP

#include <iostream>
#include <string>
#include <map>
#include <SFML/Graphics.hpp>
#include "ChessBoard.hpp"

class GUIBoard {
private:
    ChessBoard& cb;

    //const char (*board)[8];
    std::map<char, sf::Texture> textures;

public:
    // Constructor that takes a ChessBoard reference
    GUIBoard(ChessBoard& cb);

    // Function to perform additional initialization
    void initialize();

    void initializeTextures();

    void createSFMLWindow();
    void syncBoardWithGUI();
    const char(&getBoard() const)[8][8] { return cb.getBoard(); }
};

#endif // GUIBOARD_HPP