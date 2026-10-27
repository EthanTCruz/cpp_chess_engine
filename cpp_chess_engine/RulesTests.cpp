#include "RulesTests.hpp"
#include "ChessBoard.hpp"
#include "BitOps.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <chrono>

namespace {
std::string square(int index) {
    return std::string(1, static_cast<char>('a' + index % 8)) + static_cast<char>('1' + index / 8);
}
}

// Tab-separated protocol for the optional python-chess differential test.
int runRulesProbe() {
    std::string line;
    while (std::getline(std::cin, line)) {
        try {
            std::istringstream input(line);
            std::string fen, uci, san;
            std::getline(input, fen, '\t');
            std::getline(input, uci, '\t');
            std::getline(input, san, '\t');
            ChessBoard board(fen, false);
            for (const auto& entry : board.getAllMoves()) {
                int from = bitScanForward(entry.first);
                Bitboard destinations = entry.second;
                while (destinations) {
                    int to = bitScanForward(destinations);
                    std::string move = square(from) + square(to);
                    char piece = board.getBoard()[7 - from / 8][from % 8];
                    if ((piece == 'P' && to / 8 == 7) || (piece == 'p' && to / 8 == 0)) {
                        for (char promotion : std::string("qrbn")) std::cout << move << promotion << ' ';
                    } else std::cout << move << ' ';
                    destinations &= destinations - 1;
                }
            }
            ChessBoard other = board;
            std::cout << '\t' << (uci.empty() ? "-" : board.movePieceUCI(uci) ? board.getString() : "invalid");
            std::cout << '\t' << (san.empty() ? "-" : other.movePieceSAN(san) ? other.getString() : "invalid");
            ChessBoard game(fen);
            std::cout << '\t' << game.get_game_results() << '\t' << game.hasInsufficientMaterial() << '\t' << game.getEnemyAttacks() << std::endl;
        } catch (const std::exception& error) {
            std::cout << "ERROR: " << error.what() << std::endl;
        }
    }
    return 0;
}

bool runRulesTests() {
    int total = 0, failed = 0;
    auto check = [&](bool ok, const char* description) {
        ++total;
        if (!ok) { ++failed; std::cerr << "[RULES] FAIL | " << description << '\n'; }
    };
    ChessBoard initial;
    check(initial.getString() == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", "initial FEN");
    for (const std::string move : {"i2e4", "e0e4", "e2e9", "e2e4junk", "e2e4q", "e2e4k", "e2e40", "e2e2", "a7a6"}) {
        auto before = initial.getString();
        check(!initial.movePieceUCI(move) && initial.getString() == before, "invalid UCI leaves state unchanged");
    }
    check(!initial.movePiece(-1, 0, 0, 0) && !initial.validateMove(64, 0), "coordinate bounds");
    check(initial.movePieceUCI("e2e4") && initial.getString() == "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1", "double push FEN");
    check(initial.movePieceUCI("g8f6") && initial.getString().find(" w KQkq - 1 2") != std::string::npos, "EP expiry and clocks");
    ChessBoard reloaded(initial.getString());
    check(reloaded.getString() == initial.getString(), "FEN round trip");
    ChessBoard attacks("r3k3/8/8/p7/8/8/8/4K3 w - - 0 1", false);
    check(attacks.isAttacked(32) && attacks.isAttacked(40) && !attacks.isAttacked(64) && (attacks.getEnemyAttacks() & (1ULL << 32)), "attack query includes defended enemy squares and handles bounds");
    ChessBoard ep("4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1", false);
    check(ep.movePieceUCI("e5d6") && ep.getString() == "4k3/8/3P4/8/8/8/8/4K3 b - - 0 1", "EP capture removes pawn");
    ep.setString("4k3/8/8/2pP4/8/8/8/4K3 w - c6 0 1");
    check(ep.getEnPassant() == (1ULL << 42), "setString replaces EP target");
    auto validFen = ep.getString();
    for (const std::string fen : {"", "8/8/8/8/8/8/8/9 w - - 0 1", "8/8/8/8/8/8/8/7X w - - 0 1", "8/8/8/8/8/8/8/8 w - z3 0 1"}) {
        bool rejected = false;
        try { ep.setString(fen); } catch (const std::exception&) { rejected = true; }
        check(rejected && ep.getString() == validFen, "invalid FEN rejected atomically");
    }
    for (char promotion : std::string("qrbn")) {
        ChessBoard p("k7/4P3/8/8/8/8/8/4K3 w - - 0 1", false);
        check(p.movePieceUCI(std::string("e7e8") + promotion) && p.getBoard()[0][4] == std::toupper(promotion), "all promotions");
    }
    ChessBoard pawn("k7/4P3/8/8/8/8/8/4K3 w - - 0 1", false);
    check(!pawn.movePieceUCI("e7e8") && !pawn.movePieceSAN("e8") && !pawn.movePieceSAN("e8=Qjunk"), "promotion choice required");
    ChessBoard amb("4k3/8/8/8/8/8/4N1N1/4K3 w - - 0 1", false);
    check(!amb.movePieceSAN("Nf4") && amb.movePieceSAN("Nef4"), "SAN ambiguity");
    ChessBoard pinned("k3r3/8/8/8/8/8/4N1N1/4K3 w - - 0 1", false);
    check(pinned.movePieceSAN("Nf4"), "pinned candidate does not make SAN ambiguous");
    ChessBoard mate;
    check(mate.movePieceSAN("f3") && mate.movePieceSAN("e5") && mate.movePieceSAN("g4") && mate.movePieceSAN("Qh4#"), "Fools mate sequence");
    check(mate.get_game_results() == "0-1" && mate.get_is_black_win() && !mate.movePieceUCI("a2a3"), "black wins checkmate and game stops");
    ChessBoard whiteMate("7k/6Q1/5K2/8/8/8/8/8 b - - 0 1");
    check(whiteMate.get_game_results() == "1-0", "white checkmate loaded from FEN");
    ChessBoard stalemate("7k/5K2/6Q1/8/8/8/8/8 b - - 0 1");
    check(stalemate.get_is_stalemate() && stalemate.get_is_draw() && stalemate.get_game_results() == "1/2-1/2", "stalemate loaded from FEN");
    ChessBoard doubleMate("7k/6Q1/5K2/8/8/8/8/7R b - - 0 1");
    check(doubleMate.getAllMoves().empty() && doubleMate.get_game_results() == "1-0", "double-check mate has no zero-destination entries");
    ChessBoard draw("7k/8/8/8/8/8/8/K7 w - - 0 1");
    check(draw.get_is_draw() && !draw.get_is_stalemate(), "bare kings draw distinct from stalemate");
    for (const std::string fen : {"7k/8/8/8/8/8/8/KN6 w - - 0 1", "7k/8/8/8/8/8/8/KB6 w - - 0 1", "7k/8/8/8/8/4b3/8/K1B5 w - - 0 1"}) {
        ChessBoard b(fen);
        check(b.get_is_draw(), "insufficient mating material");
    }
    for (const std::string fen : {"7k/8/8/8/8/8/8/KNN5 w - - 0 1", "7k/8/8/8/8/8/8/KBN5 w - - 0 1", "7k/8/8/8/8/8/8/KBB5 w - - 0 1"}) {
        ChessBoard b(fen);
        check(!b.get_is_draw(), "possible cooperative mate must not be a material draw");
    }
    ChessBoard fifty("7k/8/8/8/8/8/R7/K7 w - - 99 1");
    check(!fifty.canClaimFiftyMoveDraw() && fifty.canClaimFiftyMoveDraw("a2b2"), "prospective fifty-move claim");
    auto claimFen = fifty.getString();
    check(fifty.claimFiftyMoveDraw("a2b2") && fifty.getString() == claimFen && fifty.get_is_draw(), "claim ends game without making intended move");
    ChessBoard hundred("7k/8/8/8/8/8/R7/K7 w - - 100 1");
    check(!hundred.get_is_game_over() && hundred.claimFiftyMoveDraw(), "fifty moves requires a claim");
    ChessBoard seventyFive("7k/8/8/8/8/8/R7/K7 w - - 149 1");
    check(seventyFive.movePieceUCI("a2b2") && seventyFive.get_is_draw(), "75 moves automatic draw");
    ChessBoard matePriority("7k/8/5KQ1/8/8/8/8/8 w - - 149 1");
    check(matePriority.movePieceUCI("g6g7") && matePriority.get_game_results() == "1-0", "mate precedes 75-move draw");
    ChessBoard pawnClock("7k/8/8/8/8/8/P7/KR6 w - - 99 1");
    check(!pawnClock.canClaimFiftyMoveDraw("a2a3") && pawnClock.movePieceUCI("a2a3") && !pawnClock.canClaimFiftyMoveDraw(), "pawn move resets fifty-move clock");
    ChessBoard captureClock("7k/8/8/8/8/8/pR6/K7 w - - 99 1");
    check(!captureClock.canClaimFiftyMoveDraw("b2a2"), "capture resets fifty-move clock");
    ChessBoard repetition;
    for (int cycle = 1; cycle <= 4; ++cycle) {
        check(repetition.movePieceUCI("g1f3") && repetition.movePieceUCI("g8f6") && repetition.movePieceUCI("f3g1") && repetition.movePieceUCI("f6g8"), "repetition cycle");
        check(repetition.get_is_draw() == (cycle == 4), "threefold disabled; fivefold automatic");
    }
    ChessBoard rights("4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1", false);
    check(rights.movePieceUCI("h1h2") && rights.getString().find(" b Q ") != std::string::npos, "rook move revokes its castling right");
    check(rights.movePieceUCI("e8e7") && rights.movePieceUCI("h2h1") && rights.getString().find(" b Q ") != std::string::npos, "rook return does not restore rights");
    ChessBoard rookCapture("4k2r/8/8/8/8/8/8/R3K2R b KQk - 0 1", false);
    check(rookCapture.movePieceUCI("h8h1") && rookCapture.getString().find(" w Q ") != std::string::npos, "rook capture revokes both relevant rights");
    ChessBoard kingRights("4k3/8/8/8/8/8/8/R3K2R w KQ - 0 1", false);
    check(kingRights.movePieceUCI("e1e2") && kingRights.getString().find(" b - ") != std::string::npos, "king move revokes both rights");
    ChessBoard castle("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1", false);
    check(castle.movePieceUCI("e1c1") && castle.getString() == "r3k2r/8/8/8/8/8/8/2KR3R b kq - 1 1", "queenside castle moves king and rook");
    check(castle.movePieceUCI("e8g8") && castle.getString() == "r4rk1/8/8/8/8/8/8/2KR3R w - - 2 2", "black kingside castle moves king and rook");
    ChessBoard stalemateMove("7k/5K2/8/6Q1/8/8/8/8 w - - 0 1");
    check(stalemateMove.movePieceUCI("g5g6") && stalemateMove.get_is_stalemate(), "move produces stalemate");
    ChessBoard doubleCheck("k3r3/8/8/8/1b6/8/8/4K3 w - - 0 1", false);
    auto doubleMoves = doubleCheck.getAllMoves();
    check(doubleMoves.size() == 1 && doubleMoves.begin()->second == ((1ULL << 3) | (1ULL << 5) | (1ULL << 13)), "double check keeps every safe king move");
    ChessBoard rightsHistory("7k/8/8/8/8/8/8/R3K3 w Q - 0 1");
    for (int cycle = 1; cycle <= 4; ++cycle) {
        check(rightsHistory.movePieceUCI("a1a2") && rightsHistory.movePieceUCI("h8h7") && rightsHistory.movePieceUCI("a2a1") && rightsHistory.movePieceUCI("h7h8"), "castling history cycle");
        check(!rightsHistory.get_is_draw(), "different castling rights distinguish repetition");
    }
    check(rightsHistory.movePieceUCI("a1a2") && rightsHistory.get_is_draw(), "fivefold triggers at first repeated intermediate position");
    for (bool legalEp : {false, true}) {
        ChessBoard epHistory(legalEp ? "4k1n1/8/8/8/3pP3/8/8/4K1N1 b - e3 0 1" :
                                      "4k1n1/8/8/8/4P3/8/8/4K1N1 b - e3 0 1");
        for (int cycle = 1; cycle <= 4; ++cycle) {
            check(epHistory.movePieceUCI("g8f6") && epHistory.movePieceUCI("g1f3") && epHistory.movePieceUCI("f6g8") && epHistory.movePieceUCI("f3g1"), "EP repetition cycle");
        }
        check(epHistory.get_is_draw() == !legalEp, "only legal en passant rights distinguish repetitions");
    }
    ChessBoard cannotWin("7k/8/8/8/8/8/R7/K7 b - - 0 1");
    cannotWin.resign("0-1");
    check(cannotWin.get_is_draw(), "resignation draws when winner cannot possibly mate");
    ChessBoard resignation;
    resignation.resign("nonsense");
    check(!resignation.get_is_game_over(), "invalid result is a no-op");
    resignation.resign("0-1");
    resignation.resign("1-0");
    check(resignation.get_game_results() == "0-1", "finished outcome immutable");
    resignation.ResetBoard();
    check(!resignation.get_is_game_over() && resignation.getString() == ChessBoard().getString(), "reset clears outcome and history");
    const auto pgnPath = std::filesystem::temp_directory_path() /
        ("chess_rules_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + ".pgn");
    auto pgnCheck = [&](const std::string& text, bool expected, const char* description) {
        { std::ofstream file(pgnPath); file << text; }
        ChessBoard validator;
        check(validator.validatePGN(pgnPath.string()) == expected, description);
    };
    pgnCheck("[Result \"1-0\"]\n\n1. f3 e5 2. g4 Qh4# 1-0\n", false, "PGN cannot override checkmate winner");
    pgnCheck("[Result \"*\"]\n\n1.e4 e5 2.Bh6 *\n", false, "PGN rejects illegal move");
    pgnCheck("[Result \"1-0\"]\n\n1.e4 e5 0-1\n", false, "PGN result header must match movetext");
    pgnCheck("[Result \"*\"]\n\n1.e4 {unclosed *\n", false, "PGN rejects unclosed comments");
    pgnCheck("[Result \"*\"]\n\n1.e4 a6 2.e5 d5 3.exd6 e.p. ;capture\n cxd6 *\n", true, "PGN annotations and en passant suffix");
    std::filesystem::remove(pgnPath);
    std::cout << "[RULES] Summary: " << total - failed << '/' << total << " passed.\n";
    return failed == 0;
}
