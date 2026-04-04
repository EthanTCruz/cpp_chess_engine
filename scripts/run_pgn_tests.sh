#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname "${BASH_SOURCE[0]}")"/.. && pwd)"
BUILD_DIR="$SCRIPT_DIR/build-tests"

if ! command -v cmake >/dev/null 2>&1; then
  echo "Error: cmake is not installed. Please install cmake and try again." >&2
  exit 1
fi

cmake -S "$SCRIPT_DIR" -B "$BUILD_DIR" -DBUILD_GUI=OFF -DBUILD_TESTING_MODULE=ON
cmake --build "$BUILD_DIR" --target cpp_chess_engine_tests

PGN_DIR="${1:-test_pgns}"
FEN_DIR="${2:-test_fens}"

if [[ "$PGN_DIR" = /* ]]; then
  PGN_PATH="$PGN_DIR"
else
  PGN_PATH="$SCRIPT_DIR/$PGN_DIR"
fi

if [[ "$FEN_DIR" = /* ]]; then
  FEN_PATH="$FEN_DIR"
else
  FEN_PATH="$SCRIPT_DIR/$FEN_DIR"
fi

"$BUILD_DIR/cpp_chess_engine_tests" "$PGN_PATH" "$FEN_PATH"
