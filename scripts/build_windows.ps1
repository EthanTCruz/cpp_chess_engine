cmake -S . -B build
cmake --build build

# Copy SFML runtime libraries next to the built executable(s)
$sfmlLibDir = "build/_deps/sfml-build/lib"
if (Test-Path $sfmlLibDir) {
    Copy-Item "$sfmlLibDir/**/*.dll" build -Recurse -Force
    if (Test-Path build/Debug) {
        Copy-Item "$sfmlLibDir/**/*.dll" build/Debug -Recurse -Force
    }
    if (Test-Path build/Release) {
        Copy-Item "$sfmlLibDir/**/*.dll" build/Release -Recurse -Force
    }
}
