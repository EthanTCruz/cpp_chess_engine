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

## Running PGN validation without the GUI

If you only need to exercise the PGN validator (for example on a
headless server) you can build and run the dedicated command line test
runner. This target does not download SFML:

```bash
./scripts/run_pgn_tests.sh
```

Any argument passed to the script is treated as the directory that
contains the PGNs to validate (defaults to `test_pgns`).

The main executable also validates every PGN file in the `test_pgns`
directory before launching the SFML interface. To skip the GUI after
validation, pass:

```bash
./cpp_chess_engine --no-gui
```

To exclusively run the PGN validation from the GUI build and return a
non-zero exit code when any PGN fails, use:

```bash
./cpp_chess_engine --pgn-tests-only
```
