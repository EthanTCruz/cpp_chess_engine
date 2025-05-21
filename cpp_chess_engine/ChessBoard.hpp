#ifndef CHESSBOARD_HPP
#define CHESSBOARD_HPP

#include <string>
#include <cstdint>
#include <array>
#include <unordered_map>
#include <iostream>
#include <cctype>
#include <bitset>
#include <sstream>
#include "Bitboard.hpp"

// The ChessBoard class manages the board state and delegates move validation.
class ChessBoard {
public:
    ChessBoard(const std::string& fen);
    std::string getString() const;
    void setString(const std::string& newFen);
    void printBoard() const;
    const char(&getBoard() const)[8][8];
    // Validate a move from 'from_idx' to 'to_idx'
    bool validateMove(const int& from_idx, const int& to_idx);
    bool isAttacked(const int& from_idx);
    Bitboard getAttacks(const int& from_idx);
    // Move a piece on the board
    bool movePiece(const int& fromRow, const int& fromCol, const int& newRow, const int& newCol);
    bool movePiece(const int& fromRow, const int& fromCol, const int& newRow, const int& newCol,const char& promote);
    // Bitboard getters for friendly pieces.
    Bitboard getWhitePieces() const;
    Bitboard getBlackPieces() const;
    bool getTurn() const;
    bool castleCheck(Bitboard from_bb, Bitboard to_bb) const;

    // Bitboard getters for knights.
    Bitboard getWhiteKnightBitboard() const;
    Bitboard getBlackKnightBitboard() const;
    Bitboard getWhiteBishopBitboard() const;
    Bitboard getWhiteRookBitboard() const ;
    Bitboard getWhiteQueenBitboard() const ;
    Bitboard getWhiteKingBitboard() const ;
    Bitboard getBlackBishopBitboard() const;
    Bitboard getBlackRookBitboard() const ;
    Bitboard getBlackQueenBitboard() const;
    Bitboard getBlackKingBitboard() const ;
    Bitboard getWhitePawnBitboard() const;
    Bitboard getBlackPawnBitboard() const;

    Bitboard getFriendlyPieces() const;
    Bitboard getEnemyPieces() const;
    Bitboard getAllPieces() const;

    Bitboard getEnPassant() const;

    std::unordered_map<Bitboard, Bitboard> getAllMoves();
    std::unordered_map<Bitboard, Bitboard> parseMoves(const std::unordered_map<Bitboard, Bitboard>& allMoves);

    Bitboard getMoves(const int& from_idx) const;

	Bitboard getCastlingRights() const;

    void castleWhiteKingside();
    void castleWhiteQueenside();
    void castleBlackQueenside();
    void castleBlackKingside();


    bool canWhiteCastleKingside() const ;
	bool canWhiteCastleQueenside() const;
	bool canBlackCastleKingside() const;
	bool canBlackCastleQueenside() const;

    void setEnPassant( int& bitIndex) ;
    void setEnPassant(const std::string& square) ;

    void syncBoardWithBitboards();
    int BoardCoordToCellIndex(const std::string& coord) const;
    bool movePieceUCI(const std::string& move);

    // Public helper to convert board coordinates to a bit index.
    int get_bitindex(int row, int col) ;

private:
    std::string fen;
    bool whiteToMove = true;
	int halfmoveClock = 0;
	int fullmoveNumber = 1;
	Bitboard enPassant = 0ULL;
    std::string castlingRights;
	bool whiteCanCastleKingside = true;
	bool whiteCanCastleQueenside = true;
	bool blackCanCastleKingside = true;
	bool blackCanCastleQueenside = true;

    //Bitboard w_king_castle = 1ULL << 62;
    //Bitboard w_queen_castle = 1ULL << 58;
    //Bitboard b_king_castle = 1ULL << 6;
    //Bitboard b_queen_castle = 1ULL << 2;

    Bitboard w_king_castle = 1ULL << 6;
    Bitboard w_queen_castle = 1ULL << 2;
    Bitboard b_king_castle = 1ULL << 62;
    Bitboard b_queen_castle = 1ULL << 58;

    char board[8][8];
    std::array<Bitboard, 12> bitboards = { 0ULL };
    std::unordered_map<char, int> piece_to_idx;

    void initialize();
    void parseFEN();

};



#endif // CHESSBOARD_HPP


