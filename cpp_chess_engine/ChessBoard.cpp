#include "ChessBoard.hpp"
#include "KnightValidator.hpp"
#include "KingValidator.hpp"
#include "PawnValidator.hpp"
#include "RookValidator.hpp"
#include "BishopValidator.hpp"
#include <intrin.h>


RookValidator rookValidator;
KingValidator kingValidator;
BishopValidator bishopValidator;

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

void printBitboardBoard(uint64_t bitboard) {
    for (int rank = 7; rank >= 0; --rank) {
        for (int file = 0; file < 8; ++file) {
            int index = rank * 8 + file;
            if (bitboard & (1ULL << index)) {
                std::cout << "1 ";
            }
            else {
                std::cout << "0 ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void printBitsetBoard(const std::bitset<64>& bitset) {
    for (int i = 63; i >= 0; --i) {
        std::cout << bitset[i] << " ";
    }
    std::cout << std::endl;
}


ChessBoard::ChessBoard(const std::string& fen) : fen(fen) {
    // Map piece characters to their corresponding bitboard index.
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
    initialize();
}

void ChessBoard::initialize() {
    // Initialize board to empty.
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            board[r][c] = '.';

    // Reset bitboards.
    bitboards.fill(0ULL);

    parseFEN();

}

void ChessBoard::setString(const std::string& newFen) {
    fen = newFen;
    initialize();

}

int ChessBoard::get_bitindex(int row, int col)  {
    // Map board coordinates to a bitboard index.
    return (7 - row) * 8 + col;
}

int get_bitindex(int row, int col) {
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
        uint64_t bb = bitboards[i];
        while (bb) {
            unsigned long index;
            // _BitScanForward64 finds the index of the least significant set bit.
            _BitScanForward64(&index, bb);
            int pos = static_cast<int>(index);
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

    std::cout << "fen:  " << fen << "\n" << "hm: " << halfmoveClockStr << "\n" << "fm: " << fullmoveNumberStr << "\n";

    // Set the halfmove clock.
    halfmoveClock = std::stoi(halfmoveClockStr);

    // Set the fullmove number.
    fullmoveNumber = std::stoi(fullmoveNumberStr);
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

uint64_t ChessBoard::getWhitePieces() const {
    return bitboards[w_pawn_idx] | bitboards[w_knight_idx] |
        bitboards[w_bishop_idx] | bitboards[w_rook_idx] |
        bitboards[w_queen_idx] | bitboards[w_king_idx];
}

uint64_t ChessBoard::getBlackPieces() const {
    return bitboards[b_pawn_idx] | bitboards[b_knight_idx] |
        bitboards[b_bishop_idx] | bitboards[b_rook_idx] |
        bitboards[b_queen_idx] | bitboards[b_king_idx];
}

uint64_t ChessBoard::getFriendlyPieces() const {
    uint64_t friendlyPieces = whiteToMove ? getWhitePieces() : getBlackPieces();
    return friendlyPieces;
}

uint64_t ChessBoard::getEnemyPieces() const {
    uint64_t enemyPieces = !whiteToMove ? getWhitePieces() : getBlackPieces();
    return enemyPieces;
}
uint64_t ChessBoard::getAllPieces() const {
    uint64_t whitePieces = getWhitePieces();
    uint64_t blackPieces= getBlackPieces();
    return whitePieces | blackPieces;
}


uint64_t ChessBoard::getWhiteKnightBitboard() const {
    return bitboards[w_knight_idx];
}
uint64_t ChessBoard::getWhiteBishopBitboard() const {
	return bitboards[w_bishop_idx];
}
uint64_t ChessBoard::getWhiteRookBitboard() const {
	return bitboards[w_rook_idx];
}
uint64_t ChessBoard::getWhiteQueenBitboard() const {
	return bitboards[w_queen_idx];
}
uint64_t ChessBoard::getWhiteKingBitboard() const {
	return bitboards[w_king_idx];
}
uint64_t ChessBoard::getBlackBishopBitboard() const {
	return bitboards[b_bishop_idx];
}
uint64_t ChessBoard::getBlackRookBitboard() const {
	return bitboards[b_rook_idx];
}
uint64_t ChessBoard::getBlackQueenBitboard() const {
	return bitboards[b_queen_idx];
}
uint64_t ChessBoard::getBlackKingBitboard() const {
	return bitboards[b_king_idx];
}

uint64_t ChessBoard::getBlackKnightBitboard() const {
    return bitboards[b_knight_idx];
}

uint64_t ChessBoard::getWhitePawnBitboard() const {
    return bitboards[w_pawn_idx];
}

uint64_t ChessBoard::getBlackPawnBitboard() const {
    return bitboards[b_pawn_idx];
}

bool ChessBoard::validateMove(const int& from_idx, const int& to_idx) {
    // Convert the bit index back to board coordinates.
    int row = 7 - (from_idx / 8);
    int col = from_idx % 8;
    char piece = board[row][col];


    // Delegate knight moves to the KnightValidator.
    if (piece == 'N' || piece == 'n') {
        KnightValidator knightValidator;
        return knightValidator.validate(from_idx, to_idx, *this);
	} else if(piece == 'P' || piece == 'p') {
		PawnValidator pawnValidator;
		return pawnValidator.validate(from_idx, to_idx, *this);
	}
	else if (piece == 'R' || piece == 'r') {
		 
		 return rookValidator.validate(from_idx, to_idx, *this);
	}
    else if (piece == 'K' || piece == 'k') {
        return kingValidator.validate(from_idx, to_idx, *this);
    }
	else if (piece == 'B' || piece == 'b') {

		return bishopValidator.validate(from_idx, to_idx, *this);
	}


    // For other pieces, you could delegate to other validators.
    // Here we simply check that the destination is not occupied by a friendly piece.
    uint64_t friendlyPieces = whiteToMove ? getWhitePieces() : getBlackPieces();
    uint64_t destination = 1ULL << to_idx;
    if (destination & friendlyPieces) {
        return false;
    }
    return true;
}

int getBitindex(int row, int col) {
    // Map board coordinates to a bitboard index.
    return (7 - row) * 8 + col;
}

uint64_t ChessBoard::getEnPassant() const {
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
    if (castlingRights.empty())
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

bool ChessBoard::movePiece(const int& fromRow, const int& fromCol, const int& newRow, const int& newCol, const char& promote) {
	if (movePiece(fromRow, fromCol, newRow, newCol)) {
		// Handle promotion if necessary.

        int to_idx = get_bitindex(newRow, newCol);

		char promote_value = !whiteToMove ? std::toupper(promote) : std::tolower(promote);
		int pawn_idx = whiteToMove ? w_pawn_idx : b_pawn_idx;

        bitboards[pawn_idx] &= ~(1ULL << to_idx);
        bitboards[piece_to_idx[promote_value]] |= (1ULL << to_idx);
        board[newRow][newCol] = promote_value;
		return true;
	}
    return false;
}

bool ChessBoard::movePiece(const int& fromRow, const int& fromCol, const int& newRow, const int& newCol) {
    int from_idx = get_bitindex(fromRow, fromCol);
    int to_idx = get_bitindex(newRow, newCol);
    //std::cout << "fromCol = " << fromCol << ";" << "\n";
    //std::cout << "fromRow = " << fromRow << ";" << "\n";
    //std::cout << "newCol = " << newCol << ";" << "\n";
    //std::cout << "newRow = " << newRow << ";" << "\n";
    //std::cout << "from_idx = " << from_idx << ";" << "\n";
    //std::cout << "to_idx = " << to_idx << ";" << "\n";

    std::string original_fen = getString();
    if (validateMove(from_idx, to_idx)) {
        char piece = board[fromRow][fromCol];
        char occupying_piece = board[newRow][newCol];
        board[fromRow][fromCol] = '.';
        board[newRow][newCol] = piece;
		bool isPawn = (piece == 'P' || piece == 'p');
        int rankEPAdjustment = whiteToMove ? -1 : 1;
        uint64_t proposed_ep = 1ULL << to_idx;

        //sets en passant


        // en passant capture
        // std::cout << "propos ep: " << std::bitset<64>(proposed_ep) << "\n" << "actual ep: " << std::bitset<64>(enPassant) << "\n";
       // std::cout << "from col: " << fromCol << "\n" << "newCol: " << newCol << "\n" << "from row: " << fromRow << "\n" << "newRow: " << newRow << "\n" << "rankEPAdjustment: " << rankEPAdjustment << "\n";
        if (isPawn && ((fromRow + (rankEPAdjustment * 2)) == newRow)) {

            int epIndex = get_bitindex(newRow - rankEPAdjustment, newCol);
            // std::cout << "Ep RESEt: \n ep row : " << newRow - rankEPAdjustment << "\n" << "ep col: " << newCol << "\n";
            setEnPassant(epIndex);
        }
        else
        if (isPawn && ((proposed_ep & enPassant) != 0)) {      
            // Remove the captured pawn from the square behind the en passant target.     
            board[newRow - rankEPAdjustment][newCol] = '.';       
			std::cout << "eraesed: " << newRow + rankEPAdjustment << " " << newCol << "\n";
            // For bitboards, use the opponent’s pawn index:       
            int oppPawnIdx = whiteToMove ? b_pawn_idx : w_pawn_idx;       
            int capture_idx = get_bitindex(newRow - rankEPAdjustment, newCol);       
            bitboards[oppPawnIdx] &= ~(1ULL << capture_idx);   
            enPassant = 0ULL;
            
        }
            
        else if (isPawn) {
            enPassant = 0ULL;
        }
          
            // Update the moving pawn’s bitboard normally.       
            printBitboardBoard(bitboards[piece_to_idx[piece]]);
            if (occupying_piece != '.') bitboards[piece_to_idx[occupying_piece]] &= ~(1ULL << to_idx);

            bitboards[piece_to_idx[piece]] &= ~(1ULL << from_idx);       
            bitboards[piece_to_idx[piece]] |= (1ULL << to_idx);   
            

            printBitboardBoard(bitboards[piece_to_idx[piece]]);



        // Change turn.
        whiteToMove = !whiteToMove;
        

        if (piece == 'p' || piece == 'P' || occupying_piece != '.') halfmoveClock += 1;
        else halfmoveClock = 0;
        
        if (whiteToMove) fullmoveNumber += 1;
        //std::string fen = getString();
        //std::cout << "original fen: " << original_fen << "\n";
        //std::cout << "fromRow = " << fromRow << ";" << "\n";
        //std::cout << "fromCol = " << fromCol << ";" << "\n";
        //std::cout << "newRow = " << newRow << ";" << "\n";
        //std::cout << "newCol = " << newCol << ";" << "\n";
        //std::cout << "fen: " << fen << "\n";
        return true;
    }
    else {
        std::cout << "Invalid move\n";
        return false;
    }
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
    move.substr(0, 1);
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



