#!/usr/bin/env python3
"""Compare legal moves, UCI/SAN transitions, FEN, and outcomes with python-chess.

Requires the existing test-data dependency: pip install python-chess.
Run after building: python scripts/check_rules_python_chess.py [test executable]
"""
import random
import subprocess
import sys
from pathlib import Path
import chess


def main():
    executable = sys.argv[1] if len(sys.argv) > 1 else "build-tests/Release/cpp_chess_engine_tests.exe"
    rng = random.Random(20261026)
    count = edges = moves_checked = 0
    process = subprocess.Popen([str(Path(executable).resolve()), "--probe"], stdin=subprocess.PIPE,
                               stdout=subprocess.PIPE, text=True, bufsize=1)

    def compare(board, move=None):
        nonlocal count, edges, moves_checked
        fen = board.fen(en_passant="fen")
        san = board.san(move) if move else ""
        uci = move.uci() if move else ""
        process.stdin.write(f"{fen}\t{uci}\t{san}\n")
        process.stdin.flush()
        response = process.stdout.readline().rstrip("\n").split("\t")
        if len(response) != 6:
            raise AssertionError((fen, response))
        actual = set(response[0].split())
        expected = {m.uci() for m in board.legal_moves}
        assert actual == expected, (fen, "missing", expected - actual, "extra", actual - expected)
        moves_checked += len(expected)
        if move:
            child = board.copy()
            child.push(move)
            expected_fen = child.fen(en_passant="fen")
            assert response[1] == expected_fen, (fen, uci, response[1], expected_fen)
            assert response[2] == expected_fen, (fen, san, response[2], expected_fen)
            edges += 1
        outcome = board.outcome(claim_draw=False)
        assert response[3] == (outcome.result() if outcome else "*"), (fen, "outcome", response[3], outcome)
        assert response[4] == str(int(board.is_insufficient_material())), (fen, "material", response[4])
        enemy_attacks = 0
        for square in chess.scan_forward(board.occupied_co[not board.turn]):
            enemy_attacks |= board.attacks_mask(square)
        assert int(response[5]) == enemy_attacks, (fen, "enemy attack map", response[5], enemy_attacks)
        count += 1

    try:
        # Every outgoing edge from hand-picked special-rule and standard perft positions.
        positions = [
            chess.STARTING_FEN,
            "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
            "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
            "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
            "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
            "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10",
            "4k3/8/8/3pP3/4K3/8/8/8 w - d6 0 1",
            "4k3/8/8/r4pPK/8/8/8/8 w - f6 0 1",
            "k7/4P3/8/8/8/8/8/4K3 w - - 0 1",
            "7k/8/5KQ1/8/8/8/8/8 w - - 149 1",
        ]
        for fen in positions:
            board = chess.Board(fen)
            compare(board)
            for move in list(board.legal_moves):
                compare(board, move)
        # Reproducible legal games, including long endgames and promotion races.
        for _ in range(100):
            board = chess.Board()
            for ply in range(240):
                legal = list(board.legal_moves)
                move = rng.choice(legal) if legal else None
                compare(board, move)
                if not move or board.is_game_over():
                    break
                board.push(move)
        # Sparse synthetic legal positions exercise checks, pins, unusual material and clocks.
        for _ in range(2500):
            while True:
                board = chess.Board(None)
                squares = rng.sample(range(64), rng.randint(2, 10))
                board.set_piece_at(squares.pop(), chess.Piece(chess.KING, chess.WHITE))
                board.set_piece_at(squares.pop(), chess.Piece(chess.KING, chess.BLACK))
                for square in squares:
                    piece = rng.choice([chess.PAWN, chess.KNIGHT, chess.BISHOP, chess.ROOK, chess.QUEEN])
                    board.set_piece_at(square, chess.Piece(piece, rng.choice([True, False])))
                board.turn = rng.choice([True, False])
                board.halfmove_clock = rng.choice([0, 99, 100, 149, 150])
                if board.is_valid():
                    break
            legal = list(board.legal_moves)
            compare(board, rng.choice(legal) if legal else None)
    finally:
        process.stdin.close()
        process.wait(timeout=10)
    print(f"PASS: {count} positions; {moves_checked} legal moves compared; {edges} UCI/SAN/FEN transitions; seed 20261026")


if __name__ == "__main__":
    main()
