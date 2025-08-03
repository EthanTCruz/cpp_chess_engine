cmake -S . -B build
cmake --build build
Copy-Item build/_deps/sfml-build/bin/*.dll build -Force
