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

After building, ensure the SFML debug or release DLLs are next to the
generated executable (e.g. copy the contents of `C:\SFML-3.0.0\bin`
into your `x64/Debug` or `x64/Release` directory) or add that `bin`
directory to your `PATH`. Without these runtime libraries, Windows will
report missing `sfml-graphics-*-3.dll` files when launching the
application.
