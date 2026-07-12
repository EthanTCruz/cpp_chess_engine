# Agent Guide

Update this file after any changes made by the LLM.

## Project Shape

- C++17 chess engine with optional SFML GUI.
- Root build files: `CMakeLists.txt`, `CMakeSettings.json`, `cpp_chess_engine.sln`.
- Engine source lives in `cpp_chess_engine/`.
- Test data lives in `test_pgns/` and `test_fens/`.
- Perft move-count data lives in `test_perft/`.
- Shared test settings live in `test_config.env`; environment variables override matching keys.
- Helper scripts live in `scripts/`.
- Treat `build/`, `build-pgn/`, `build-tests/`, `out/`, `.vs/`, and `x64/` as generated/local output.

## Core Files

- `cpp_chess_engine/ChessBoard.hpp` / `.cpp`: board state, FEN parsing, legal move application, SAN/UCI moves, castling, en passant, check/game result state.
- `cpp_chess_engine/Bitboard.hpp`: `uint64_t` bitboard type and magic metadata.
- `cpp_chess_engine/BitOps.hpp` / `.cpp`: bitboard helpers.
- `cpp_chess_engine/*Validator.hpp` / `.cpp`: piece-specific move validation.
- `cpp_chess_engine/MoveValidator.hpp`: validator interface.
- `cpp_chess_engine/Constants.hpp`: file/rank masks and initial/castling constants.
- `cpp_chess_engine/PGNTestRunner.*`: validates PGN files.
- `cpp_chess_engine/FenMoveTester.*`: loads CSV FEN/SAN move tests and runs them.
- `cpp_chess_engine/PerftTester.*`: loads CSV perft tests and counts legal move nodes up to `PERFT_MAX_DEPTH`.
- `cpp_chess_engine/testing_main.cpp`: headless test executable entrypoint.
- `cpp_chess_engine/main.cpp` and `GUIBoard.*`: SFML GUI executable path.
- `docs/ENGINE.md`: engine module and algorithm reference, including bitboard operations by move type.

## Build And Test

Prefer the headless test path for engine changes:

```powershell
.\scripts\run_tests_windows.ps1
```

Optional parameters:

```powershell
.\scripts\run_tests_windows.ps1 -PgnDirectory test_pgns -FenDirectory test_fens -BuildDir build-tests
```

Equivalent direct CMake flow:

```powershell
cmake -S . -B build-tests -DBUILD_GUI=OFF -DBUILD_TESTING_MODULE=ON
cmake --build build-tests --target cpp_chess_engine_tests --config Debug
.\build-tests\Debug\cpp_chess_engine_tests.exe test_pgns test_fens
```

GUI build:

```powershell
.\scripts\build_windows.ps1
```

Linux/macOS:

```bash
./scripts/build_linux.sh
./scripts/run_pgn_tests.sh
```

## Test Data

- FEN move tests are CSV rows under `test_fens/`:
  `fen,san_move,should_succeed,description`
- `should_succeed` accepts values like `true/false`, `1/0`, `pass/fail`, and `success/failure`.
- Lines starting with `#` are comments.
- Regenerate the default FEN test CSV with:

```bash
python scripts/generate_fen_tests.py
```

## Development Notes

- Keep engine logic independent of SFML; put GUI-only work in `GUIBoard.*` or `main.cpp`.
- When changing move legality, inspect `ChessBoard`, the relevant piece validator, and both PGN/FEN tests.
- When changing engine architecture, move generation, or bitboard algorithms, update `docs/ENGINE.md`.
- Prefer adding a focused CSV case in `test_fens/` for move-rule regressions.
- Prefer adding or updating PGNs in `test_pgns/` for game-sequence regressions.
- `scripts/generate_fen_tests.py` is the source for the default comprehensive FEN/SAN suite; regenerate `test_fens/default_fen_move_tests.csv` after editing it.
- `scripts/generate_perft_tests.py` uses `python-chess` to generate `test_perft/perft_tests.csv`; `PERFT_MAX_DEPTH` comes from the environment or `test_config.env`.
- Promotion handling accepts only queen, rook, bishop, and knight (`q/r/b/n`) in SAN and UCI paths.
- Castling rules use separate occupancy and attack masks; queenside `b1`/`b8` must be empty but may be attacked.
- Enemy queen attacks must include both bishop-like and rook-like rays; check-evasion masks must allow capturing pawn checkers.
- The headless test runner prints `[PERF]` lines for PGN, FEN, PERFT, and overall runtime; use those as before/after performance reports.
- Do not edit generated build outputs unless the user explicitly asks.
- Keep changes small and consistent with the existing C++ style; avoid new dependencies unless they are clearly needed.
