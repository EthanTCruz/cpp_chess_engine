#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname "${BASH_SOURCE[0]}")"/.. && pwd)"
BUILD_DIR="$SCRIPT_DIR/build-pgn"

if ! command -v cmake >/dev/null 2>&1; then
  echo "Error: cmake is not installed. Please install cmake and try again." >&2
  exit 1
fi

cmake -S "$SCRIPT_DIR" -B "$BUILD_DIR" -DBUILD_GUI=OFF -DBUILD_PGN_TEST_RUNNER=ON
cmake --build "$BUILD_DIR" --target pgn_tester

PGN_DIR="${1:-test_pgns}"
if [[ "$PGN_DIR" = /* ]]; then
  PGN_PATH="$PGN_DIR"
else
  PGN_PATH="$SCRIPT_DIR/$PGN_DIR"
fi

"$BUILD_DIR/pgn_tester" "$PGN_PATH"
