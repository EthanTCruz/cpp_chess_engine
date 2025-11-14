#include "ChessBoard.hpp"
#include "KnightValidator.hpp"
#include "KingValidator.hpp"
#include "PawnValidator.hpp"
#include "RookValidator.hpp"
#include "BishopValidator.hpp"
#include "Constants.hpp"
#include "BitOps.hpp"
#include <fstream>
#include <sstream>
#include <vector>


RookValidator rookValidator;
KingValidator kingValidator;
BishopValidator bishopValidator;
KnightValidator knightValidator;
PawnValidator pawnValidator;

// Bitboard index constants.
namespace {
    const int w_pawn_idx = 0;
    const int w_knight_idx = 1;
    const int w_bishop_idx = 2;
    const int w_rook_idx = 3;
    const int w_queen_idx = 4;
    const int w_king_idx = 5;

    const int b_pawn_idx = 6;
    const int b_knight_idx = 7;
    const int b_bishop_idx = 8;
    const int b_rook_idx = 9;
    const int b_queen_idx = 10;
    const int b_king_idx = 11;


}



std::string printBitboardBoard(Bitboard bitboard) {
    std::string bboard = "";
    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            int index = rank * 8 + file;
            if (bitboard & (1ULL << index)) {
                bboard += "1 ";
                std::cout << "1 ";
            }
            else {
                bboard += "0 ";
                std::cout << "0 ";
            }
        }
        bboard += "\n";
        std::cout << std::endl;
    }
    bboard += "\n";
    std::cout << std::endl;

    return bboard;
}

void printBitsetBoard(const std::bitset<64>& bitset) {
    for (int i = 63; i >= 0; --i) {
        std::cout << bitset[i] << " ";
    }
    std::cout << std::endl;
}


bool ChessBoard::get_is_game_over(){
return is_game_over;
}
bool ChessBoard::get_is_white_win(){
return is_white_win;
}
bool ChessBoard::get_is_black_win(){
return is_black_win;
}
bool ChessBoard::get_is_stalemate(){
return is_stalemate;
}

std::string ChessBoard::get_game_results(){
if (!is_game_over) return "*";
if (is_white_win) return "1-0";
if (is_black_win) return "0-1";
if (is_stalemate) return "1/2-1/2";
return "Error";
}

void ChessBoard::resign(const std::string& resultToken) {
    // Reset existing game result flags before applying the resignation outcome.
    is_white_win = false;
    is_black_win = false;
    is_stalemate = false;
    is_game_over = false;

    if (resultToken == "1-0") {
        is_white_win = true;
        is_game_over = true;
    }
    else if (resultToken == "0-1") {
        is_black_win = true;
        is_game_over = true;
    }
    else if (resultToken == "1/2-1/2") {
        is_stalemate = true;
        is_game_over = true;
    }
    else {
        // Unrecognized result tokens should not change the game state.
        is_game_over = false;
    }
}

ChessBoard::ChessBoard(const std::string& fen) : fen(fen) {
    // Map piece characters to their corresponding bitboard index.

    initialize();
}

ChessBoard::ChessBoard() {
    // Map piece characters to their corresponding bitboard index.
	fen = STARTING_FEN;
    initialize();
}

void ChessBoard::ResetBoard() {
    // Map piece characters to their corresponding bitboard index.
    fen = STARTING_FEN;
    initialize();
}


void ChessBoard::initialize() {

    piece_to_idx = {
    {'P', w_pawn_idx},
    {'N', w_knight_idx},
    {'B', w_bishop_idx},
    {'R', w_rook_idx},
    {'Q', w_queen_idx},
    {'K', w_king_idx},
    {'p', b_pawn_idx},
    {'n', b_knight_idx},
    {'b', b_bishop_idx},
    {'r', b_rook_idx},
    {'q', b_queen_idx},
    {'k', b_king_idx}
    };
    // Initialize board to empty.
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            board[r][c] = '.';

    is_game_over = false;
    is_white_win = false;
    is_black_win = false;
    is_stalemate = false;
    // Reset bitboards.
    bitboards.fill(0ULL);
    
    parseFEN();


}



Bitboard ChessBoard::getWhiteKnightBitboard() const {
    return bitboards[w_knight_idx];
}
Bitboard ChessBoard::getWhiteBishopBitboard() const {
    return bitboards[w_bishop_idx];
}
Bitboard ChessBoard::getWhiteRookBitboard() const {
    return bitboards[w_rook_idx];
}
Bitboard ChessBoard::getWhiteQueenBitboard() const {
    return bitboards[w_queen_idx];
}
Bitboard ChessBoard::getWhiteKingBitboard() const {
    return bitboards[w_king_idx];
}
Bitboard ChessBoard::getBlackBishopBitboard() const {
    return bitboards[b_bishop_idx];
}
Bitboard ChessBoard::getBlackRookBitboard() const {
    return bitboards[b_rook_idx];
}
Bitboard ChessBoard::getBlackQueenBitboard() const {
    return bitboards[b_queen_idx];
}
Bitboard ChessBoard::getBlackKingBitboard() const {
    return bitboards[b_king_idx];
}

Bitboard ChessBoard::getBlackKnightBitboard() const {
    return bitboards[b_knight_idx];
}

Bitboard ChessBoard::getWhitePawnBitboard() const {
    return bitboards[w_pawn_idx];
}

Bitboard ChessBoard::getBlackPawnBitboard() const {
    return bitboards[b_pawn_idx];
}

Bitboard ChessBoard::getPawnBitboards() const {
    return bitboards[b_pawn_idx] | bitboards[w_pawn_idx];
}

Bitboard ChessBoard::getKnightBitboards() const {
	return bitboards[w_knight_idx] | bitboards[b_knight_idx];
}

Bitboard ChessBoard::getBishopBitboards() const {
	return bitboards[w_bishop_idx] | bitboards[b_bishop_idx];
}

Bitboard ChessBoard::getRookBitboards() const {
	return bitboards[w_rook_idx] | bitboards[b_rook_idx];
}

Bitboard ChessBoard::getQueenBitboards() const {
	return bitboards[w_queen_idx] | bitboards[b_queen_idx];
}

Bitboard ChessBoard::getKingBitboards() const {
	return bitboards[w_king_idx] | bitboards[b_king_idx];
}

void ChessBoard::setString(const std::string& newFen) {
    fen = newFen;
    initialize();

}

int ChessBoard::get_bitindex(int row, int col)  {
    // Map board coordinates to a bitboard index.
    return (7 - row) * 8 + col;
}




void ChessBoard::syncBoardWithBitboards() {
    // Clear the board.
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            board[r][c] = '.';
        }
    }

    // Map each bitboard index to its corresponding piece character.
    static const char pieceChars[12] = { 'P', 'N', 'B', 'R', 'Q', 'K',
                                           'p', 'n', 'b', 'r', 'q', 'k' };

    // Iterate through each bitboard.
    for (int i = 0; i < 12; ++i) {
        Bitboard bb = bitboards[i];
        while (bb) {
            int pos = bitScanForward(bb);
            // Convert bit index to board coordinates.
            int row = 7 - (pos / 8);
            int col = pos % 8;
            // Place the corresponding piece on the board.
            board[row][col] = pieceChars[i];
            // Clear the least significant set bit.
            bb &= bb - 1;
        }
    }
}



void ChessBoard::parseFEN() {
    std::istringstream fenStream(fen);
    std::string piecePlacement, activeColor, castling, enPassantSquare, halfmoveClockStr, fullmoveNumberStr;

    fenStream >> piecePlacement >> activeColor >> castling >> enPassantSquare >> halfmoveClockStr >> fullmoveNumberStr;

    int row = 0, col = 0;
    for (char ch : piecePlacement) {
        if (ch == '/') {
            row++;
            col = 0;
        }
        else if (std::isdigit(ch)) {
            int count = ch - '0';
            for (int i = 0; i < count; ++i) {
                if (col < 8) {
                    board[row][col++] = '.';
                }
            }
        }
        else {
            if (row < 8 && col < 8) {
                board[row][col] = ch;
                int bit_index = get_bitindex(row, col);
                bitboards[piece_to_idx[ch]] |= 1ULL << bit_index;
                col++;
            }
        }
    }

    // Set the active color.
    whiteToMove = (activeColor == "w");

    // Set the castling rights.

    castlingRights = castling;

    whiteCanCastleKingside = castling.find('K') != std::string::npos;
    whiteCanCastleQueenside = castling.find('Q') != std::string::npos;
    blackCanCastleKingside = castling.find('k') != std::string::npos;
    blackCanCastleQueenside = castling.find('q') != std::string::npos;

    // Set the en passant target square.
    if (enPassantSquare != "-") {
        int enPassantIndex = BoardCoordToCellIndex(enPassantSquare);
        if (enPassantIndex != -1) {
            enPassant |= 1ULL << enPassantIndex;
        }
    }
    else {
        enPassant = 0;
    }

    // Set the halfmove clock.
    halfmoveClock = std::stoi(halfmoveClockStr);

    // Set the fullmove number.
    fullmoveNumber = std::stoi(fullmoveNumberStr);

    if (whiteToMove) {
		friendlyPieces = getWhitePieces();
		enemyPieces = getBlackPieces();
    }
    else {
		friendlyPieces = getBlackPieces();
		enemyPieces = getWhitePieces();
    }
}




const char (&ChessBoard::getBoard() const)[8][8] {
    return board;
}


void ChessBoard::printBoard() const {
    std::cout << "Board state:\n";
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            std::cout << board[r][c] << " ";
        }
        std::cout << "\n";
    }
}

bool ChessBoard::getTurn() const {
    return whiteToMove;
}

Bitboard ChessBoard::getWhitePieces() const {
    return bitboards[w_pawn_idx] | bitboards[w_knight_idx] |
        bitboards[w_bishop_idx] | bitboards[w_rook_idx] |
        bitboards[w_queen_idx] | bitboards[w_king_idx];
}

Bitboard ChessBoard::getBlackPieces() const {
    return bitboards[b_pawn_idx] | bitboards[b_knight_idx] |
        bitboards[b_bishop_idx] | bitboards[b_rook_idx] |
        bitboards[b_queen_idx] | bitboards[b_king_idx];
}

Bitboard ChessBoard::getFriendlyPieces() const {
    Bitboard friendlyPieces = whiteToMove ? getWhitePieces() : getBlackPieces();
    return friendlyPieces;
}

Bitboard ChessBoard::getEnemyPieces() const {
    Bitboard enemyPieces = !whiteToMove ? getWhitePieces() : getBlackPieces();
    return enemyPieces;
}

Bitboard ChessBoard::getAllPieces() const {
    Bitboard whitePieces = getWhitePieces();
    Bitboard blackPieces= getBlackPieces();
    return whitePieces | blackPieces;
}




bool ChessBoard::canWhiteCastleKingside(Bitboard attacks, Bitboard all_pieces) const{
    Bitboard invalid_conditions = attacks | all_pieces;
    invalid_conditions &= w_king_castle_conditions;
    bool can_castle = invalid_conditions ? whiteCanCastleKingside : false;
    
	return can_castle;
}
bool ChessBoard::canWhiteCastleQueenside(Bitboard attacks, Bitboard all_pieces) const {
	return whiteCanCastleQueenside;
}
bool ChessBoard::canBlackCastleKingside(Bitboard attacks, Bitboard all_pieces) const {
	return blackCanCastleKingside;
}
bool ChessBoard::canBlackCastleQueenside(Bitboard attacks, Bitboard all_pieces) const {
	return blackCanCastleQueenside;
}





bool ChessBoard::validateMove(const int& from_idx, const int& to_idx) {
    // Convert the bit index back to board coordinates.
    int row = 7 - (from_idx / 8);
    int col = from_idx % 8;
    char piece = board[row][col];
	Bitboard origin = 1ULL << from_idx;
    Bitboard destination = 1ULL << to_idx;

    std::unordered_map<Bitboard, Bitboard> allMoves = getAllMoves();

    if (allMoves.find(origin) != allMoves.end()) {
        return allMoves[origin] & destination;
    }

	//if (~getFriendlyPieces() & origin) return false; 
 //   
 //   if (origin & (bitboards[w_knight_idx] | bitboards[b_knight_idx])) return knightValidator.validate(from_idx, destination, *this);

 //   else if (origin & (bitboards[w_pawn_idx] | bitboards[b_pawn_idx])) return pawnValidator.validate(origin, destination, *this);

 //   else if (origin & (bitboards[w_rook_idx] | bitboards[b_rook_idx])) return rookValidator.validate(from_idx, destination, *this);

 //   else if (origin & (bitboards[w_king_idx] | bitboards[b_king_idx])) /*return kingValidator.validate(from_idx, destination, *this);*/
 //   {
 //       if (kingValidator.validate(from_idx, destination, *this)) return true;
 //       else return (castleCheck(origin, destination));
 //   }
 //           


 //   
	//else if (origin & (bitboards[w_bishop_idx] | bitboards[b_bishop_idx])) return bishopValidator.validate(from_idx, destination, *this);
	//
 //   else if (origin & (bitboards[w_queen_idx] | bitboards[b_queen_idx])) {
 //       if (bishopValidator.validate(from_idx, destination, *this)) return true;
 //       return rookValidator.validate(from_idx, destination, *this);

 //   }


    return false;
}

int getBitindex(int row, int col) {
    // Map board coordinates to a bitboard index.
    return (7 - row) * 8 + col;
}

Bitboard ChessBoard::getEnPassant() const {
	return enPassant;
}

void ChessBoard::setEnPassant( int& bitIndex)  {
    if (bitIndex >= 0 && bitIndex < 64) {
        enPassant = 1ULL << bitIndex;
    }
}

void ChessBoard::setEnPassant(const std::string& square)  {
	int bitIndex = BoardCoordToCellIndex(square);
	setEnPassant(bitIndex);
}

std::string ChessBoard::getString() const {
    std::string fenString;
    // Build the piece placement portion.
    for (int row = 0; row < 8; ++row) {
        int emptyCount = 0;
        for (int col = 0; col < 8; ++col) {
            int bitIndex = getBitindex(row, col);
            char piece = '.';
            // Order of indices: 0:'P', 1:'N', 2:'B', 3:'R', 4:'Q', 5:'K',
            // 6:'p', 7:'n', 8:'b', 9:'r', 10:'q', 11:'k'
            static const char pieceChar[12] = { 'P', 'N', 'B', 'R', 'Q', 'K',
                                                'p', 'n', 'b', 'r', 'q', 'k' };
            bool foundPiece = false;
            for (int i = 0; i < 12; ++i) {
                if (bitboards[i] & (1ULL << bitIndex)) {
                    piece = pieceChar[i];
                    foundPiece = true;
                    break;
                }
            }
            if (!foundPiece || piece == '.') {
                ++emptyCount;
            }
            else {
                if (emptyCount > 0) {
                    fenString += std::to_string(emptyCount);
                    emptyCount = 0;
                }
                fenString.push_back(piece);
            }
        }
        if (emptyCount > 0) {
            fenString += std::to_string(emptyCount);
        }
        if (row != 7)
            fenString.push_back('/');
    }

    // Append active color.
    fenString.push_back(' ');
    fenString.push_back(whiteToMove ? 'w' : 'b');

    // Append castling rights.
    fenString.push_back(' ');
    // If castlingRights is empty, use '-' as per FEN convention.
	std::string castlingRights = "";

    castlingRights += whiteCanCastleKingside ? "K" : "";
    castlingRights += whiteCanCastleQueenside ? "Q" : "";
    castlingRights += blackCanCastleKingside ? "k" : "";
    castlingRights += blackCanCastleQueenside ? "q" : "";

    if (castlingRights == "")
        fenString.push_back('-');
    else
        fenString += castlingRights;

    // Append en passant target square.
    // (Not implemented here so we use '-' as a placeholder.)
    fenString.push_back(' ');
    fenString.push_back('-');

    // Append halfmove clock.
    fenString.push_back(' ');
    fenString += std::to_string(halfmoveClock);

    // Append fullmove number.
    fenString.push_back(' ');
    fenString += std::to_string(fullmoveNumber);

    return fenString;
}

bool ChessBoard::movePiece(const int& fromRow, const int& fromCol,
                           const int& newRow, const int& newCol,
                           const char& promote) {
    return movePieceInternal(fromRow, fromCol, newRow, newCol, promote, true);
}

void ChessBoard::changeTurn() {
    whiteToMove = !whiteToMove;

    if (whiteToMove) {
        friendlyPieces = getWhitePieces();
        enemyPieces = getBlackPieces();
    } else {
        friendlyPieces = getBlackPieces();
        enemyPieces = getWhitePieces();
    }
}

void ChessBoard::switchTurn() {
    // Switch the turn without modifying clocks or move counters.
    whiteToMove = !whiteToMove;

    if (whiteToMove) {
        friendlyPieces = getWhitePieces();
        enemyPieces = getBlackPieces();
    } else {
        friendlyPieces = getBlackPieces();
        enemyPieces = getWhitePieces();
    }
}


bool ChessBoard::movePiece(const int& fromRow, const int& fromCol,
                           const int& newRow, const int& newCol) {
    return movePieceInternal(fromRow, fromCol, newRow, newCol, '0', true);
}

bool ChessBoard::movePieceUnchecked(const int& fromRow, const int& fromCol,
                                    const int& newRow, const int& newCol,
                                    char promote) {
    return movePieceInternal(fromRow, fromCol, newRow, newCol, promote, false);
}

bool ChessBoard::movePieceInternal(const int& fromRow, const int& fromCol,
                                   const int& newRow, const int& newCol,
                                   char promote, bool validateMoveFlag) {
    int from_idx = get_bitindex(fromRow, fromCol);
    int to_idx = get_bitindex(newRow, newCol);
    Bitboard origin = 1ULL << from_idx;
    Bitboard destination = 1ULL << to_idx;

    if (validateMoveFlag && !validateMove(from_idx, to_idx)) {
        return false;
    }

    bool movingWhite = whiteToMove;

    if (castleCheck(origin, destination)) {
        if (whiteToMove) {
            if (destination & w_king_castle) {
                castleWhiteKingside();
            } else {
                castleWhiteQueenside();
            }
        } else {
            if (destination & b_king_castle) {
                castleBlackKingside();
            } else {
                castleBlackQueenside();
            }
        }
    }

    char piece = board[fromRow][fromCol];
    char occupying_piece = board[newRow][newCol];
    board[fromRow][fromCol] = '.';
    board[newRow][newCol] = piece;

    bool isPawn = (piece == 'P' || piece == 'p');
    int rankEPAdjustment = whiteToMove ? -1 : 1;
    Bitboard proposed_ep = destination;

    if (origin & InitialPositions::wk_rook) whiteCanCastleKingside = false;
    if (origin & InitialPositions::wq_rook) whiteCanCastleQueenside = false;
    if (origin & InitialPositions::bk_rook) blackCanCastleKingside = false;
    if (origin & InitialPositions::bq_rook) blackCanCastleQueenside = false;

    if (origin & InitialPositions::w_king) {
        whiteCanCastleKingside = false;
        whiteCanCastleQueenside = false;
    }
    if (origin & InitialPositions::b_king) {
        blackCanCastleKingside = false;
        blackCanCastleQueenside = false;
    }

    if (isPawn && ((fromRow + (rankEPAdjustment * 2)) == newRow)) {
        int epIndex = get_bitindex(newRow - rankEPAdjustment, newCol);
        setEnPassant(epIndex);
    } else if (isPawn && ((proposed_ep & enPassant) != 0)) {
        board[newRow - rankEPAdjustment][newCol] = '.';
        int oppPawnIdx = whiteToMove ? b_pawn_idx : w_pawn_idx;
        int capture_idx = get_bitindex(newRow - rankEPAdjustment, newCol);
        bitboards[oppPawnIdx] &= ~(1ULL << capture_idx);
        enPassant = 0ULL;
    } else {
        enPassant = 0ULL;
    }

    if (occupying_piece != '.') {
        if (destination & InitialPositions::wk_rook) whiteCanCastleKingside = false;
        if (destination & InitialPositions::wq_rook) whiteCanCastleQueenside = false;
        if (destination & InitialPositions::bk_rook) blackCanCastleKingside = false;
        if (destination & InitialPositions::bq_rook) blackCanCastleQueenside = false;

        bitboards[piece_to_idx[occupying_piece]] &= ~(1ULL << to_idx);
    }

    bitboards[piece_to_idx[piece]] &= ~(1ULL << from_idx);
    bitboards[piece_to_idx[piece]] |= (1ULL << to_idx);

    if (isPawn && ((fromRow + (rankEPAdjustment * 2)) != newRow)) {
        enPassant = 0ULL;
    }

    if (promote != '0') {
        char promote_value = movingWhite ? std::toupper(promote) : std::tolower(promote);
        int pawn_idx = movingWhite ? w_pawn_idx : b_pawn_idx;
        bitboards[pawn_idx] &= ~(1ULL << to_idx);
        bitboards[piece_to_idx[promote_value]] |= (1ULL << to_idx);
        board[newRow][newCol] = promote_value;
    }

    if (piece == 'p' || piece == 'P' || occupying_piece != '.') {
        halfmoveClock = 0;
    } else {
        halfmoveClock += 1;
    }

    if (!movingWhite) {
        fullmoveNumber += 1;
    }

    changeTurn();

    std::unordered_map<Bitboard, Bitboard> all_moves = getAllMoves();
    if (all_moves.empty()) {
        Bitboard enemy_attacks = getEnemyAttacks();
        if (enemy_attacks & getFriendlyPieces() & getKingBitboards()) {
            if (whiteToMove) {
                is_white_win = true;
            } else {
                is_black_win = true;
            }
        } else {
            is_stalemate = true;
        }
        is_game_over = true;
    }

    return true;
}

int const RankToRow(const char& rank)  {
	return  ('8' - rank);
}

int const FileToCol(const char& file) {
	return (file - 'a');
}

int ChessBoard::BoardCoordToCellIndex(const std::string& coord) const {
    if (coord.length() < 2) {
		std::cout << "Invalid coordinate format: " << coord << "\n";
        return -1;
    }
    int col = FileToCol(coord[0]);
    int row = RankToRow(coord[1]);


	int idx = getBitindex(row, col);
	return idx;
}
void ChessBoard::castleWhiteKingside() {
    bitboards[w_king_idx] = bitboards[w_king_idx] << 2;
    Bitboard initial_rook_position = 1ULL << 7;
    Bitboard new_rook_position = 1ULL << 5;
    bitboards[w_rook_idx] = (bitboards[w_rook_idx] & ~initial_rook_position) | new_rook_position;
    board[7][7] = '.';
    board[7][5] = 'R';
}
void ChessBoard::castleWhiteQueenside() {
    bitboards[w_king_idx] = bitboards[w_king_idx] >> 2;
    Bitboard initial_rook_position = 1ULL << 0;
    Bitboard new_rook_position = 1ULL << 3;
    bitboards[w_rook_idx] = (bitboards[w_rook_idx] & ~initial_rook_position) | new_rook_position;
    board[7][0] = '.';
    board[7][3] = 'R';
}
void ChessBoard::castleBlackQueenside() {
    bitboards[b_king_idx] = bitboards[b_king_idx] >> 2;
    Bitboard initial_rook_position = 1ULL << 56;
    Bitboard new_rook_position = 1ULL << 59;
    bitboards[b_rook_idx] = (bitboards[b_rook_idx] & ~initial_rook_position) | new_rook_position;
    board[0][0] = '.';
    board[0][3] = 'r';
}
void ChessBoard::castleBlackKingside() {
    bitboards[b_king_idx] = bitboards[b_king_idx] << 2;
    Bitboard initial_rook_position = 1ULL << 63;
    Bitboard new_rook_position = 1ULL << 61;
    bitboards[b_rook_idx] = (bitboards[b_rook_idx] & ~initial_rook_position) | new_rook_position;
    board[0][7] = '.';
    board[0][5] = 'r';
}



bool ChessBoard::castleCheck(Bitboard from_bb, Bitboard to_bb) const {
    Bitboard initial_position = InitialPositions::b_king;
    Bitboard kingside_target = b_king_castle;
    Bitboard queenside_target = b_queen_castle;
    bool has_kingside_castle_permissions = blackCanCastleKingside;
    bool has_queenside_castle_permissions = blackCanCastleQueenside;

    if (whiteToMove) {
        initial_position = InitialPositions::w_king;
        kingside_target = w_king_castle;
        queenside_target = w_queen_castle;
        has_kingside_castle_permissions = whiteCanCastleKingside;
        has_queenside_castle_permissions = whiteCanCastleQueenside;
    }

    Bitboard validStart = from_bb & initial_position;
    if (!validStart) return false;

    bool isKingside = (to_bb & kingside_target) != 0ULL;
    bool isQueenside = (to_bb & queenside_target) != 0ULL;
    if (!isKingside && !isQueenside) return false;

    if (isKingside && !has_kingside_castle_permissions) return false;
    if (isQueenside && !has_queenside_castle_permissions) return false;

    return true;
}

// goal is to get ~w_king_castle then & with king moves
Bitboard ChessBoard::validCastles(Bitboard attacks, Bitboard all_pieces) const {
    // to start get castle move squares
    Bitboard valid_castles = b_king_castle | b_queen_castle;
    Bitboard k_castle = b_king_castle;
    Bitboard q_castle = b_queen_castle;

    // next get which squares need to be clear of attacks and pieces for castling
    Bitboard k_castle_condition = b_king_castle_conditions;
    Bitboard q_castle_condition = b_queen_castle_conditions;

    if (whiteToMove) {
        k_castle_condition = w_king_castle_conditions;
        q_castle_condition = w_queen_castle_conditions;

        k_castle = w_king_castle;
        q_castle = w_queen_castle;

        valid_castles = w_king_castle | w_queen_castle;
    }

    // if invalid conditions == 0 or false, then valid
    Bitboard invalid_conditions = attacks | all_pieces;
    invalid_conditions &= (k_castle_condition | q_castle_condition);


    return ~valid_castles;
}


Bitboard ChessBoard::getCastlingRights()  {
    Bitboard initial_position = InitialPositions::b_king;
    Bitboard target_position = b_king_castle | b_queen_castle;
	Bitboard king_position = bitboards[b_king_idx];
    Bitboard k_castle = b_king_castle;
    Bitboard q_castle = b_queen_castle;

    Bitboard k_castle_condition = b_king_castle_conditions;
    Bitboard q_castle_condition = b_queen_castle_conditions;

	bool can_castle_kingside = blackCanCastleKingside;
	bool can_castle_queenside = blackCanCastleQueenside;

    if (whiteToMove) {
        initial_position = InitialPositions::w_king;
        target_position = w_king_castle | w_queen_castle;
        king_position = bitboards[w_king_idx];

        k_castle = w_king_castle;
        q_castle = w_queen_castle;

		can_castle_kingside = whiteCanCastleKingside;
		can_castle_queenside = whiteCanCastleQueenside;

        k_castle_condition = w_king_castle_conditions;
        q_castle_condition = w_queen_castle_conditions;
    }

    Bitboard validStart = king_position & initial_position;
	Bitboard castling_rights = validStart ? target_position : 0ULL;

	Bitboard enemy_pieces = getEnemyPieces();
    Bitboard attacks = getEnemyAttacks();
    Bitboard friendly_pieces = getFriendlyPieces();

	if (((attacks | friendly_pieces) & k_castle_condition) || !can_castle_kingside) {
		castling_rights &= ~k_castle;
	}
	if (((attacks | friendly_pieces) & q_castle_condition) || !can_castle_queenside) {
        
		castling_rights &= ~q_castle;
	}

    return castling_rights;

}

Bitboard ChessBoard::getAttacks(const int& from_idx) {
	Bitboard enemy_pieces = getEnemyPieces();
    Bitboard king_attacks = kingValidator.getAttacks(from_idx, *this) & (enemy_pieces & (bitboards[w_king_idx] | bitboards[b_king_idx]));
    Bitboard bishop_attacks = bishopValidator.getAttacks(from_idx, *this) & (enemy_pieces & (bitboards[w_bishop_idx] | bitboards[b_bishop_idx] | bitboards[w_queen_idx] | bitboards[b_queen_idx]));
    Bitboard rook_attacks = rookValidator.getAttacks(from_idx, *this) & (enemy_pieces & (bitboards[w_rook_idx] | bitboards[b_rook_idx] | bitboards[w_queen_idx] | bitboards[b_queen_idx]));
    Bitboard knight_attacks = knightValidator.getAttacks(from_idx, *this) & (enemy_pieces & (bitboards[w_knight_idx] | bitboards[b_knight_idx]));
    Bitboard pawn_attacks = pawnValidator.getAttacks(1Ull << from_idx, *this) & (enemy_pieces & (bitboards[w_pawn_idx] | bitboards[b_pawn_idx]));
    

	return king_attacks | bishop_attacks | rook_attacks | knight_attacks | pawn_attacks;
}

Bitboard ChessBoard::getMoves(const int& from_idx) {
    int row = 7 - (from_idx / 8);
    int col = from_idx % 8;
    char piece = board[row][col];
    Bitboard origin = 1ULL << from_idx;
    Bitboard moves = 0ULL;
    Bitboard friendly_pieces = getFriendlyPieces();

    if (~getFriendlyPieces() & origin) return false;
    origin &= friendly_pieces;

    Bitboard knight_moves = origin & (bitboards[w_knight_idx] | bitboards[b_knight_idx]);
    Bitboard pawn_moves = origin & (bitboards[w_pawn_idx] | bitboards[b_pawn_idx]);
    Bitboard rook_moves = origin & (bitboards[w_rook_idx] | bitboards[b_rook_idx]);
    Bitboard king_moves = origin & (bitboards[w_king_idx] | bitboards[b_king_idx]);
    Bitboard bishop_moves = origin & (bitboards[w_bishop_idx] | bitboards[b_bishop_idx]);
	Bitboard queen_moves = origin & (bitboards[w_queen_idx] | bitboards[b_queen_idx]);

    if (knight_moves) moves |= knightValidator.getAttacks(from_idx, *this);
	if (pawn_moves) moves |= pawnValidator.getMoves(origin, *this);
	if (rook_moves) moves |= rookValidator.getAttacks(from_idx, *this);
    if (king_moves) moves |= kingValidator.getAttacks(from_idx, *this); // | getCastlingRights();
	if (bishop_moves) moves |= bishopValidator.getAttacks(from_idx, *this);
	if (queen_moves) moves |= rookValidator.getAttacks(from_idx, *this) | bishopValidator.getAttacks(from_idx, *this);



    return moves;
}

std::unordered_map<Bitboard, Bitboard> ChessBoard::getAllMoves()  {
        std::unordered_map<Bitboard, Bitboard> allMoves;
        Bitboard friendly_pieces = getFriendlyPieces();

    while (friendly_pieces) {
        int index = bitScanForward(friendly_pieces);  // Index of least significant '1' bit
        Bitboard isolatedBit = 1ULL << index;
                Bitboard attacks = getMoves(index);
        if (isolatedBit & (getKingBitboards() & friendly_pieces)) attacks |= getCastlingRights();

		allMoves[isolatedBit] = attacks;

        friendly_pieces &= friendly_pieces - 1;  // Clear the least significant '1' bit
    }

    allMoves = parseMoves(allMoves);

    return allMoves;
}

//bool hasPinnedPiece(Bitboard pin_ray) {
//    return (pin_ray & (pin_ray - 1)) == 0;
//}

bool hasPinnedPiece(Bitboard pin_ray) {
    return (pin_ray != 0ULL) && ((pin_ray & (pin_ray - 1)) == 0ULL);
}

Bitboard ChessBoard::getEnemyAttacksIgnoringKing()  {
    Bitboard enemy_attacks = 0ULL;
    Bitboard enemy_pieces = getEnemyPieces();
    Bitboard friendly_pieces = getFriendlyPieces();


	Bitboard w_king = bitboards[w_king_idx];
	Bitboard b_king = bitboards[b_king_idx];

    Bitboard tmp_king = 0ULL;
    
    if (whiteToMove) {
        tmp_king = bitboards[w_king_idx];
        bitboards[w_king_idx] = 0ULL;
    }
    else {

        tmp_king = bitboards[b_king_idx];
        bitboards[b_king_idx] = 0ULL;
    }

    while (enemy_pieces) {
        int index = bitScanForward(enemy_pieces);
        Bitboard isolatedBit = 1ULL << index;
        Bitboard attacks = getMoves(index);



        if (isolatedBit & (bitboards[w_king_idx] | bitboards[b_king_idx])) {
            enemy_attacks |= kingValidator.getAttacks(index, *this);
        }
        else if (isolatedBit & (bitboards[w_bishop_idx] | bitboards[b_bishop_idx] | bitboards[w_queen_idx] | bitboards[b_queen_idx])) {
            enemy_attacks |= bishopValidator.getAttacks(index, *this);
        }
        else if (isolatedBit & (bitboards[w_rook_idx] | bitboards[b_rook_idx] | bitboards[w_queen_idx] | bitboards[b_queen_idx])) {
            enemy_attacks |= rookValidator.getAttacks(index, *this);
        }
        else if (isolatedBit & (bitboards[w_knight_idx] | bitboards[b_knight_idx])) {
            enemy_attacks |= knightValidator.getAttacks(index, *this);
        }
        else if (isolatedBit & (bitboards[w_pawn_idx] | bitboards[b_pawn_idx])) {
            switchTurn();
            enemy_attacks |= pawnValidator.getAttacks(isolatedBit, *this);
            switchTurn();
        }


        enemy_pieces &= enemy_pieces - 1;
    }

    if (whiteToMove) {
        bitboards[w_king_idx] = tmp_king;
    }
    else {
        bitboards[b_king_idx] = tmp_king;
    }

	return enemy_attacks;
}


Bitboard ChessBoard::getEnemyAttacks() {
    Bitboard enemy_attacks = 0ULL;
    Bitboard enemy_pieces = getEnemyPieces();

    switchTurn();
    while (enemy_pieces) {
        int index = bitScanForward(enemy_pieces);
        Bitboard isolatedBit = 1ULL << index;
        Bitboard attacks = getMoves(index);



        if (isolatedBit & (bitboards[w_king_idx] | bitboards[b_king_idx])) {
            enemy_attacks |= kingValidator.getAttacks(index, *this);
        }
        else if (isolatedBit & (bitboards[w_bishop_idx] | bitboards[b_bishop_idx] | bitboards[w_queen_idx] | bitboards[b_queen_idx])) {
            enemy_attacks |= bishopValidator.getAttacks(index, *this);
        }
        else if (isolatedBit & (bitboards[w_rook_idx] | bitboards[b_rook_idx] | bitboards[w_queen_idx] | bitboards[b_queen_idx])) {
            enemy_attacks |= rookValidator.getAttacks(index, *this);
        }
        else if (isolatedBit & (bitboards[w_knight_idx] | bitboards[b_knight_idx])) {
            enemy_attacks |= knightValidator.getAttacks(index, *this);
        }
        else if (isolatedBit & (bitboards[w_pawn_idx] | bitboards[b_pawn_idx])) {
            //switchTurn();
            enemy_attacks |= pawnValidator.getAttacks(isolatedBit, *this);
            //switchTurn();
        }


        enemy_pieces &= enemy_pieces - 1;
    }
    switchTurn();
    return enemy_attacks;
}


std::unordered_map<Bitboard, Bitboard> ChessBoard::removeBishopPins(const std::unordered_map<Bitboard, Bitboard>& allMoves) {
    std::unordered_map<Bitboard, Bitboard> parsedMoves = allMoves;
    Bitboard queens = (bitboards[w_queen_idx] | bitboards[b_queen_idx]);
    Bitboard enemy_pieces = getEnemyPieces();
    Bitboard enemy_bishops = enemy_pieces & (bitboards[b_bishop_idx] | bitboards[w_bishop_idx] | queens);
	Bitboard friendly_pieces = getFriendlyPieces();

    Bitboard friendly_king = friendly_pieces & (bitboards[w_king_idx] | bitboards[b_king_idx]);
    int friendly_king_idx = bitScanForward(friendly_king);
    Bitboard occupancy = getAllPieces();

    while (enemy_bishops) {
        int enemy_bishop_idx = bitScanForward(enemy_bishops);  // Index of least significant '1' bit
        //Bitboard isolated_bishop = 1ULL << enemy_bishop_idx;

        //Bitboard king_threats = bishopValidator.getBishopAttacks(friendly_king_idx, occupancy);
        //Bitboard bishop_attacks = bishopValidator.getBishopAttacks(enemy_bishop_idx, occupancy);

        //Bitboard pin_lane = bishop_attacks & king_threats | isolated_bishop;

		Bitboard pin_lane = getBishopPinLanes(enemy_bishop_idx, friendly_king_idx, occupancy);

        Bitboard pinned_pieces = pin_lane & friendly_pieces;



        if (hasPinnedPiece(pinned_pieces)) {
            int pinned_piece_idx = bitScanForward(pinned_pieces);  // Index of least significant '1' bit
            Bitboard pinned_piece = 1ULL << pinned_piece_idx;
            if (parsedMoves.find(pinned_piece) != parsedMoves.end()) {

                parsedMoves[pinned_piece] &= pin_lane;  // Restrict moves to the pin lane
                if (parsedMoves[pinned_piece] == 0ULL) {  // If no moves left, remove it
                    parsedMoves.erase(pinned_piece);  // Remove pinned piece from possible moves
                }


            }
        }




        enemy_bishops &= enemy_bishops - 1;  // Clear the least significant '1' bit
    }
	return parsedMoves;
}

Bitboard ChessBoard::getBishopPinLanes(int bishop_idx, int target_idx, Bitboard occupancy) {
    Bitboard bishop_bb = 1ULL << bishop_idx;
    Bitboard king_bb = 1ULL << target_idx;

    int bishop_rank = bishop_idx / 8;
    int bishop_file = bishop_idx % 8;
    int king_rank = target_idx / 8;
    int king_file = target_idx % 8;

    int rank_diff = bishop_rank - king_rank;
    int file_diff = bishop_file - king_file;
    int abs_rank_diff = rank_diff < 0 ? -rank_diff : rank_diff;
    int abs_file_diff = file_diff < 0 ? -file_diff : file_diff;

    if (abs_rank_diff != abs_file_diff || abs_rank_diff == 0)
        return 0ULL;

    int step_rank = (rank_diff > 0) ? 1 : -1;
    int step_file = (file_diff > 0) ? 1 : -1;

    Bitboard friendly_mask = getFriendlyPieces() & ~king_bb;
    Bitboard lane = 0ULL;
    Bitboard friendly_on_lane = 0ULL;

    int cur_rank = king_rank + step_rank;
    int cur_file = king_file + step_file;

    while (cur_rank >= 0 && cur_rank < 8 && cur_file >= 0 && cur_file < 8) {
        int cur_idx = cur_rank * 8 + cur_file;
        Bitboard cur_bb = 1ULL << cur_idx;

        lane |= cur_bb;

        if (cur_idx == bishop_idx) {
            break;
        }

        if (occupancy & cur_bb) {
            if (friendly_mask & cur_bb) {
                friendly_on_lane |= cur_bb;
            }
            else {
                return 0ULL;
            }
        }

        cur_rank += step_rank;
        cur_file += step_file;
    }

    if ((lane & bishop_bb) == 0ULL)
        return 0ULL;

    if (!hasPinnedPiece(friendly_on_lane))
        return 0ULL;

    return lane;
}

Bitboard ChessBoard::getRookPinLanes(int rook_idx, int target_idx, Bitboard occupancy) {
    Bitboard rook_bb = 1ULL << rook_idx;
    Bitboard king_bb = 1ULL << target_idx;

    int rook_rank = rook_idx / 8;
    int rook_file = rook_idx % 8;
    int king_rank = target_idx / 8;
    int king_file = target_idx % 8;

    int rank_diff = rook_rank - king_rank;
    int file_diff = rook_file - king_file;

    if (rank_diff != 0 && file_diff != 0)
        return 0ULL;

    int step_rank = (rank_diff > 0) ? 1 : (rank_diff < 0 ? -1 : 0);
    int step_file = (file_diff > 0) ? 1 : (file_diff < 0 ? -1 : 0);

    Bitboard friendly_mask = getFriendlyPieces() & ~king_bb;
    Bitboard lane = 0ULL;
    Bitboard friendly_on_lane = 0ULL;

    int cur_rank = king_rank + step_rank;
    int cur_file = king_file + step_file;

    while (cur_rank >= 0 && cur_rank < 8 && cur_file >= 0 && cur_file < 8) {
        int cur_idx = cur_rank * 8 + cur_file;
        Bitboard cur_bb = 1ULL << cur_idx;

        lane |= cur_bb;

        if (cur_idx == rook_idx) {
            break;
        }

        if (occupancy & cur_bb) {
            if (friendly_mask & cur_bb) {
                friendly_on_lane |= cur_bb;
            }
            else {
                return 0ULL;
            }
        }

        cur_rank += step_rank;
        cur_file += step_file;
    }

    if ((lane & rook_bb) == 0ULL)
        return 0ULL;

    if (!hasPinnedPiece(friendly_on_lane))
        return 0ULL;

    return lane;
    // return 0ULL;
}

std::unordered_map<Bitboard, Bitboard> ChessBoard::removeRookPins(const std::unordered_map<Bitboard, Bitboard>& allMoves) {
    std::unordered_map<Bitboard, Bitboard> parsedMoves = allMoves;
    Bitboard queens = (bitboards[w_queen_idx] | bitboards[b_queen_idx]);
    Bitboard enemy_pieces = getEnemyPieces();
    Bitboard friendly_pieces = getFriendlyPieces();

    Bitboard friendly_king = friendly_pieces & (bitboards[w_king_idx] | bitboards[b_king_idx]);
    int friendly_king_idx = bitScanForward(friendly_king);
    Bitboard occupancy = getAllPieces();

    Bitboard enemy_rooks = enemy_pieces & (bitboards[b_rook_idx] | bitboards[w_rook_idx] | queens);


    while (enemy_rooks) {
        int enemy_rook_idx = bitScanForward(enemy_rooks);  // Index of least significant '1' bit
        


		Bitboard pin_lane = getRookPinLanes( enemy_rook_idx, friendly_king_idx, occupancy);

        Bitboard pinned_pieces = pin_lane & friendly_pieces;

        if (hasPinnedPiece(pinned_pieces)) {
            int pinned_piece_idx = bitScanForward(pinned_pieces);  // Index of least significant '1' bit
            Bitboard pinned_piece = 1ULL << pinned_piece_idx;
            if (parsedMoves.find(pinned_piece) != parsedMoves.end()) {

                parsedMoves[pinned_piece] &= pin_lane;  // Restrict moves to the pin lane
                if (parsedMoves[pinned_piece] == 0ULL) {  // If no moves left, remove it
                    parsedMoves.erase(pinned_piece);  // Remove pinned piece from possible moves
                }


            }
        }




        enemy_rooks &= enemy_rooks - 1;  // Clear the least significant '1' bit
    }
    return parsedMoves;
}

std::unordered_map<Bitboard, Bitboard> ChessBoard::parseMoves(const std::unordered_map<Bitboard, Bitboard>& allMoves) {
   std::unordered_map<Bitboard, Bitboard> parsedMoves = allMoves; 
   //king move error happens before here
   Bitboard friendly_pieces = getFriendlyPieces();
   Bitboard friendly_king = friendly_pieces & (bitboards[w_king_idx] | bitboards[b_king_idx]);
   int friendly_king_idx = bitScanForward(friendly_king);



   parsedMoves = removeBishopPins(parsedMoves); // Remove bishop pins first

   parsedMoves = removeRookPins(parsedMoves); // Remove rook pins after bishop pins

   Bitboard enemy_attacks = getEnemyAttacksIgnoringKing();

   if (parsedMoves.find(friendly_king) != parsedMoves.end()) {
       parsedMoves[friendly_king] &= ~enemy_attacks;  // Remove all moves that are attacked by enemy pieces
       if (parsedMoves[friendly_king] == 0ULL) {  // If no moves left, remove it
           parsedMoves.erase(friendly_king);  // Remove king move from possible moves
       }
   }
   Bitboard king_checkers = getAttacks(friendly_king_idx);
   Bitboard enemy_pieces = getEnemyPieces();
// for move in parsed moves, if 

   if (king_checkers) {
       parsedMoves[friendly_king] &= ~enemy_attacks;

       const Bitboard whiteCastlingSquares = w_king_castle | w_queen_castle;
       const Bitboard blackCastlingSquares = b_king_castle | b_queen_castle;

       if (friendly_king & (InitialPositions::b_king & bitboards[b_king_idx] | InitialPositions::w_king & bitboards[w_king_idx])) {
       parsedMoves[friendly_king] &= ~CastlingTrigger::all;
        }

       if (king_checkers & (king_checkers - 1)) {
            // create a new parsed moves hash map with only the king moves 
           if (parsedMoves.find(friendly_king) == parsedMoves.end()){
			   std::cout << "Checkmate \n";
                is_game_over = true;
           }
		   std::unordered_map<Bitboard, Bitboard> king_moves;
		   king_moves[friendly_king] = parsedMoves[friendly_king] & king_checkers; // Only keep moves that are valid for the king
		   parsedMoves = king_moves; // Replace parsed moves with king moves
		   return parsedMoves; // Return early if the king is in check with multiple attackers
       }


        

           Bitboard bishop_checks = king_checkers & enemy_pieces & getBishopBitboards();
           Bitboard rook_checks = king_checkers & enemy_pieces & getRookBitboards();
           Bitboard pawn_checks = king_checkers & enemy_pieces & getPawnBitboards();
           Bitboard knight_checks = king_checkers & enemy_pieces & getKnightBitboards();
           Bitboard queen_checks = king_checkers & enemy_pieces & getQueenBitboards();
		   //Bitboard friendly_pieces = getFriendlyPieces();
           friendly_pieces = getFriendlyPieces();
           
           if (rook_checks | queen_checks) {
               int rook_idx = bitScanForward(rook_checks | queen_checks);
                //should just return bitboard where pins are possible, is returning 0 which is an issue
               Bitboard pin_lane = getRookPinLanes(rook_idx, friendly_king_idx, getAllPieces());
               
               
               //pin_lane |= rook_checks; // Include the rook checks in the pin lane unless it is already included ?
               if (pin_lane != 0ULL) {
                   while (friendly_pieces) {
                       int index = bitScanForward(friendly_pieces);  // Index of least significant '1' bit
                       Bitboard isolatedBit = 1ULL << index;
                       if (index != friendly_king_idx) {
                           if (parsedMoves.find(isolatedBit) != parsedMoves.end()) {
                               parsedMoves[isolatedBit] &= pin_lane | rook_checks;  // Restrict moves to the pin lane
                               if (parsedMoves[isolatedBit] == 0ULL) {
                                   //If no moves left, remove it
                                   parsedMoves.erase(isolatedBit);  // Remove pinned piece from possible moves
                               }
                           }
                       }
                       else {
                           if (parsedMoves.find(isolatedBit) != parsedMoves.end()) {
                               parsedMoves[isolatedBit] &= ((~pin_lane | rook_checks | queen_checks) & ~enemy_attacks);  // Restrict moves to the pin lane
                               if (parsedMoves[isolatedBit] == 0ULL) {
                                   //If no moves left, remove it
                                   parsedMoves.erase(isolatedBit);  // Remove pinned piece from possible moves
                               }
                           }
                       }

                       friendly_pieces &= friendly_pieces - 1;   //Clear the least significant '1' bit
                   }
               }
           }

               friendly_pieces = getFriendlyPieces();
               if (bishop_checks | queen_checks | pawn_checks) {
                   int bishop_idx = bitScanForward(bishop_checks | queen_checks | pawn_checks);

                   Bitboard pin_lane = getBishopPinLanes(bishop_idx, friendly_king_idx, getAllPieces());

                   //pin_lane |= bishop_checks; // Include the rook checks in the pin lane unless it is already included ?

                   if (pin_lane != 0ULL) {
                       while (friendly_pieces) {
                           int index = bitScanForward(friendly_pieces);  // Index of least significant '1' bit
                           Bitboard isolatedBit = 1ULL << index;
                           if (index != friendly_king_idx) {
                               if (parsedMoves.find(isolatedBit) != parsedMoves.end()) {
                                   parsedMoves[isolatedBit] &= pin_lane | bishop_checks;  // Restrict moves to the pin lane
                                   if (parsedMoves[isolatedBit] == 0ULL) {
                                       //If no moves left, remove it
                                       parsedMoves.erase(isolatedBit);  // Remove pinned piece from possible moves
                                   }
                               }
                           }
                           else {
                               if (parsedMoves.find(isolatedBit) != parsedMoves.end()) {
                                   parsedMoves[isolatedBit] &= ((~pin_lane | bishop_checks | queen_checks) & ~enemy_attacks);  // Restrict moves to the pin lane
                                   if (parsedMoves[isolatedBit] == 0ULL) {
                                       //If no moves left, remove it
                                       parsedMoves.erase(isolatedBit);  // Remove pinned piece from possible moves
                                   }
                               }
                           }
                           friendly_pieces &= friendly_pieces - 1;   //Clear the least significant '1' bit

                       }
                   }

               }
   }

   return parsedMoves;
}


bool ChessBoard::isAttacked(const int& from_idx) {
    // Convert the bit index back to board coordinates.
    int row = 7 - (from_idx / 8);
    int col = from_idx % 8;
    char piece = board[row][col];
    Bitboard origin = 1ULL << from_idx;

    // Change turn
    
    Bitboard attacks = getAttacks(from_idx);
    // Revert turn
    

    return false;
    return true;
}

bool ChessBoard::movePieceUCI(const std::string& move) {
    // A valid UCI move must be at least 4 characters (e.g., "e2e4")
    if (move.length() < 4) {
        std::cerr << "Invalid UCI move format: " << move << std::endl;
        return false;
    }
    // Parse source and destination from the UCI string.
    // Files: 'a' -> 0, 'b' -> 1, ... 'h' -> 7.
    // Ranks: '1'-'8' with row = 8 - (rank value) because row 0 is rank 8.
    int fromCol = (move[0] - 'a');
    int fromRow = '8' - move[1];
    // move.substr(0, 1);
    int toCol =  move[2] - 'a';
    int toRow = '8' - (move[3]);


    char promotion = (move.length() == 5 ? move[4] : '0');
	//std::cout << "fromCol: " << fromCol << "\n";
	//std::cout << "fromRow: " << fromRow << "\n";
	//std::cout << "toCol: " << toCol << "\n";
	//std::cout << "toRow: " << toRow << "\n";
	//std::cout << "promotion: " << promotion << "\n";
 //   std::cout << "============================" << "\n";
    

    //// Check for out-of-bound values.
    //if (fromFile < 0 || fromFile > 7 || toFile < 0 || toFile > 7 ||
    //    fromRank < 1 || fromRank > 8 || toRank < 1 || toRank > 8) {
    //    std::cerr << "UCI move out of board bounds: " << move << std::endl;
    //    return false;
    //}

    //int fromRow = 8 - fromRank; // Convert rank to row index.
    //int fromCol = fromFile;
    //int toRow = 8 - toRank;
    //int toCol = toFile;


    // Call the existing movePiece function.
	if (promotion != '0') return movePiece(fromRow, fromCol, toRow, toCol, promotion);
	else
    return movePiece(fromRow, fromCol, toRow, toCol);
}

bool ChessBoard::movePieceSAN(const std::string& sanMove) {
    std::string move = sanMove;
    syncBoardWithBitboards();

    while (!move.empty() && (move.back() == '+' || move.back() == '#' ||
                             move.back() == '!' || move.back() == '?')) {
        move.pop_back();
    }

    if (move == "O-O" || move == "0-0") {
        return whiteToMove ? movePieceUCI("e1g1") : movePieceUCI("e8g8");
    }
    if (move == "O-O-O" || move == "0-0-0") {
        return whiteToMove ? movePieceUCI("e1c1") : movePieceUCI("e8c8");
    }

    char promotion = '0';
    size_t eqPos = move.find('=');
    if (eqPos != std::string::npos && eqPos + 1 < move.size()) {
        promotion = static_cast<char>(std::tolower(move[eqPos + 1]));
        move = move.substr(0, eqPos);
    }

    size_t capturePos = move.find('x');
    bool isCapture = capturePos != std::string::npos;

    char pieceChar = 'P';
    size_t idx = 0;
    if (!move.empty() && std::isupper(move[0]) && move[0] != 'O') {
        pieceChar = move[0];
        idx = 1;
    }

    if (move.size() < 2) {
        return false;
    }

    std::string dest = move.substr(move.size() - 2, 2);
    int toCol = dest[0] - 'a';
    int toRow = '8' - dest[1];
    if (toCol < 0 || toCol >= 8 || toRow < 0 || toRow >= 8) {
        return false;
    }

    int destIndex = get_bitindex(toRow, toCol);
    Bitboard destMask = 1ULL << destIndex;

    size_t disambEnd = (isCapture ? capturePos : move.size() - 2);
    std::string disamb = move.substr(idx, disambEnd - idx);

    auto matchesDisambiguation = [&](int row, int col) {
        if (disamb.empty()) return true;
        if (disamb.size() == 2) {
            return col == disamb[0] - 'a' && row == ('8' - disamb[1]);
        }
        if (std::isdigit(disamb[0])) {
            return row == ('8' - disamb[0]);
        }
        return col == disamb[0] - 'a';
    };

    auto canReach = [&](int row, int col) {
        char boardPiece = board[row][col];
        bool isWhitePiece = std::isupper(boardPiece);
        char targetPiece = board[toRow][toCol];

        if (isCapture) {
            if (targetPiece == '.') {
                if (pieceChar != 'P') {
                    if ((enPassant & destMask) == 0ULL) return false;
                } else if ((enPassant & destMask) == 0ULL) {
                    return false;
                }
            } else if ((std::isupper(targetPiece) && isWhitePiece) ||
                       (std::islower(targetPiece) && !isWhitePiece)) {
                return false;
            }
        } else {
            if (targetPiece != '.') {
                return false;
            }
        }

        int fromIdx = get_bitindex(row, col);

        switch (pieceChar) {
            case 'P':
                return pawnValidator.validate(1ULL << fromIdx, destMask, *this);
            case 'N':
                return knightValidator.validate(fromIdx, destIndex, *this);
            case 'B':
                return bishopValidator.validate(fromIdx, destMask, *this);
            case 'R':
                return rookValidator.validate(fromIdx, destMask, *this);
            case 'Q':
                return bishopValidator.validate(fromIdx, destMask, *this) ||
                       rookValidator.validate(fromIdx, destMask, *this);
            case 'K':
                return kingValidator.validate(fromIdx, destIndex, *this);
            default:
                return false;
        }
    };

    char requiredPiece = (pieceChar == 'P') ? 'P' : pieceChar;
    bool movingWhite = whiteToMove;

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            char boardPiece = board[row][col];
            if (boardPiece == '.') continue;
            bool isWhitePiece = std::isupper(boardPiece);
            if (isWhitePiece != whiteToMove) continue;
            if (std::toupper(boardPiece) != requiredPiece) continue;

            if (!matchesDisambiguation(row, col)) continue;
            if (!canReach(row, col)) continue;

            ChessBoard trial = *this;
            if (!trial.movePieceInternal(row, col, toRow, toCol, promotion, false)) {
                continue;
            }

            trial.switchTurn();
            Bitboard opponentAttacks = trial.getEnemyAttacks();
            trial.switchTurn();

            Bitboard kingBB = movingWhite ? trial.bitboards[w_king_idx] : trial.bitboards[b_king_idx];
            if (opponentAttacks & kingBB) {
                continue;
            }

            *this = trial;
            return true;
        }
    }

    return false;
}

bool ChessBoard::validatePGN(const std::string& pgnPath) {
    std::ifstream file(pgnPath);
    if (!file.is_open()) {
        std::cerr << "Unable to open PGN file: " << pgnPath << std::endl;
        return false;
    }
    bool allCorrect = true;
    std::string line;
    std::string expectedResult;
    std::string currentGameId;
    std::vector<std::string> moveTokens;

    auto processGame = [&]() {
        if (moveTokens.empty()) return;
        ChessBoard game; // start from initial position
        size_t moveIndex = 0;

        auto normalizeToken = [](std::string token) {
            if (!token.empty() && token.back() == '\r') token.pop_back();
            return token;
        };

        auto detectResultToken = [&](size_t idx, std::string& canonical) -> bool {
            if (idx >= moveTokens.size()) return false;
            std::string current = normalizeToken(moveTokens[idx]);
            if (current == "*") {
                canonical = current;
                return true;
            }
            if (current == "1-0" || current == "0-1" || current == "1/2-1/2") {
                canonical = current;
                return true;
            }

            auto nextToken = [&](size_t offset) {
                if (idx + offset < moveTokens.size()) {
                    return normalizeToken(moveTokens[idx + offset]);
                }
                return std::string();
            };

            std::string combined = current;
            for (size_t offset = 1; offset <= 2; ++offset) {
                std::string next = nextToken(offset);
                if (next.empty()) break;
                combined += next;
                if (combined == "1-0" || combined == "0-1" || combined == "1/2-1/2") {
                    canonical = combined;
                    return true;
                }
            }
            return false;
        };

        for (size_t i = 0; i < moveTokens.size(); ++i) {
            std::string resultToken;
            if (detectResultToken(i, resultToken)) {
                if (resultToken != "*") {
                    game.resign(resultToken);
                }
                break;
            }

            std::string previous_fen = game.getString();
            std::string tok = normalizeToken(moveTokens[i]);
            if (tok.find('.') != std::string::npos) continue; // skip move numbers
            if (tok.size() == 1 && i + 1 < moveTokens.size()) {
                tok += normalizeToken(moveTokens[++i]);
            }

            ++moveIndex;
            if (!game.movePieceSAN(tok)) {
                std::cerr << "Game " << currentGameId
                          << " Initial FEN: "
                          << previous_fen << '\n'
                          << " failed at move " << moveIndex
                          << " (" << tok << ")"
                          << "FEN: "
                          << game.getString() << std::endl;
                allCorrect = false;
                break;
            }
        }
        std::string actual = game.get_game_results();
        if (actual != expectedResult) {
            std::cerr << "Game " << currentGameId
                      << " result mismatch: expected " << expectedResult
                      << " but got " << actual << std::endl;
            allCorrect = false;
        }
        moveTokens.clear();
        expectedResult.clear();
        currentGameId.clear();
        game.ResetBoard();
    };

    while (std::getline(file, line)) {
        if (line.empty()) {
            processGame();
            continue;
        }
        if (line[0] == '[') {
            if (line.rfind("[Result", 0) == 0) {
                size_t first = line.find('"');
                size_t second = line.find('"', first + 1);
                if (first != std::string::npos && second != std::string::npos)
                    expectedResult = line.substr(first + 1, second - first - 1);
            } else if (line.rfind("[GameId", 0) == 0) {
                size_t first = line.find('"');
                size_t second = line.find('"', first + 1);
                if (first != std::string::npos && second != std::string::npos)
                    currentGameId = line.substr(first + 1, second - first - 1);
            }
            continue;
        }
        std::stringstream ss(line);
        std::string token;
        while (ss >> token) {
            moveTokens.push_back(token);
        }
    }
    processGame();
    return allCorrect;
}



