# cpp_chess_engine

## Build

This project uses [CMake](https://cmake.org/) and automatically downloads its SFML dependency.
To build the project you need a C++ compiler and CMake installed on your system.

### Linux/macOS
Run the build script directly (do not source it):

```bash
./scripts/build_linux.sh
```

On Linux, SFML additionally requires the `libudev` headers. You can install them with:

```bash
sudo apt install libudev-dev
```

### Windows
```powershell
.\scripts\build_windows.ps1
```

The build script also copies SFML runtime DLLs and piece images into common
output directories (`build`, `out/build/x64-*`, and `x64/*`) so the executable
can be launched from Visual Studio CMake mode or legacy solution mode.

If Windows still reports a missing `sfml-graphics-*.dll` file, verify your
startup target points at one of those output directories, or add the SFML
runtime folder to your `PATH`.

## Running the centralized testing module (no GUI / no SFML)

A dedicated executable, `cpp_chess_engine_tests`, now centralizes test execution for:

- PGN validation (`PGNTestRunner`)
- FEN+move validation (`FenMoveTester`) loaded from CSV files in `test_fens`

It is intentionally headless and can be run/debugged independently from `cpp_chess_engine.exe`.

### Linux/macOS

```bash
./scripts/run_pgn_tests.sh
```

You can pass a PGN directory (defaults to `test_pgns`):

```bash
./scripts/run_pgn_tests.sh custom_pgns custom_fens
```

### Windows PowerShell

Use the new script to build and run tests without SFML:

```powershell
.\scripts\run_tests_windows.ps1
```

Optional parameters:

```powershell
.\scripts\run_tests_windows.ps1 -PgnDirectory test_pgns -FenDirectory test_fens -BuildDir build-tests
```

### Debugging in Visual Studio

1. Configure with CMake options: `-DBUILD_GUI=OFF -DBUILD_TESTING_MODULE=ON`
2. Set startup item to `cpp_chess_engine_tests.exe`
3. Set command arguments to `test_pgns test_fens` (or custom directories)

### Suggested best-practice test format

For FEN move tests, add one or more CSV files under `test_fens/` with rows:

`fen,san_move,should_succeed,description`

- `fen`: full FEN (including side-to-move/castling/en-passant fields)
- `san_move`: SAN move to attempt
- `should_succeed`: `true/false`, `1/0`, `pass/fail`, or `success/failure`
- `description`: optional explanatory text

Lines beginning with `#` are treated as comments, and files are loaded in filename order for deterministic runs.
