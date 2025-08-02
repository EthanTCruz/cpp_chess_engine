# cpp_chess_engine

## Build

This project uses [CMake](https://cmake.org/) and automatically downloads its SFML dependency.
To build the project you need a C++ compiler and CMake installed on your system.

### Linux/macOS
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
