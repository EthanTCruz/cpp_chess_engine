#!/usr/bin/env python3
"""Generate depth-limited perft tests with python-chess."""

from __future__ import annotations

import csv
import os
from pathlib import Path

import chess

OUTPUT = Path("test_perft/perft_tests.csv")
CONFIG = Path("test_config.env")


def config_value(name: str, default: int) -> int:
    if name in os.environ:
        return int(os.environ[name])

    if CONFIG.exists():
        for line in CONFIG.read_text(encoding="utf-8").splitlines():
            line = line.strip()
            if not line or line.startswith("#") or "=" not in line:
                continue
            key, value = line.split("=", 1)
            if key.strip() == name:
                return int(value.strip())

    return default

POSITIONS = [
    (chess.STARTING_FEN, "Starting position"),
    ("rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2", "Open game after 1...e5"),
    ("rnbq1k1r/pppp1ppp/5n2/4p3/2B1P1b1/5N2/PPPP1PPP/RNBQK2R w KQ - 4 4", "Developed opening with castling rights"),
    ("r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1", "Castling and rook-check stress position"),
    ("4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1", "En passant available"),
]


def perft(board: chess.Board, depth: int) -> int:
    if depth == 0:
        return 1

    nodes = 0
    for move in board.legal_moves:
        board.push(move)
        nodes += perft(board, depth - 1)
        board.pop()
    return nodes


def main() -> None:
    max_depth = config_value("PERFT_MAX_DEPTH", 2)
    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    with OUTPUT.open("w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["fen", "depth", "expected_nodes", "description"])
        for fen, description in POSITIONS:
            board = chess.Board(fen)
            for depth in range(1, max_depth + 1):
                writer.writerow([fen, depth, perft(board, depth), description])

    print(f"Wrote {len(POSITIONS) * max_depth} perft test cases to {OUTPUT}")


if __name__ == "__main__":
    main()
