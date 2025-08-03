cmake -S . -B build
cmake --build build

# Copy SFML runtime libraries next to the built executable(s)
Copy-Item build/_deps/sfml-build/bin/**/*.dll build -Recurse -Force
if (Test-Path build/Debug) {
    Copy-Item build/_deps/sfml-build/bin/**/*.dll build/Debug -Recurse -Force
}
if (Test-Path build/Release) {
    Copy-Item build/_deps/sfml-build/bin/**/*.dll build/Release -Recurse -Force
}
