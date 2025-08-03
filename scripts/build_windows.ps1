cmake -S . -B build
cmake --build build

# Copy SFML runtime libraries next to the built executable(s)
$sfmlRoot = "build/_deps/sfml-build"
$sfmlBin = Join-Path $sfmlRoot "bin"
$sfmlLib = Join-Path $sfmlRoot "lib"

if (Test-Path $sfmlBin) {
    $sfmlDlls = Get-ChildItem -Path $sfmlBin -Filter '*.dll' -Recurse
} elseif (Test-Path $sfmlLib) {
    $sfmlDlls = Get-ChildItem -Path $sfmlLib -Filter '*.dll' -Recurse
}

if ($sfmlDlls) {
    $sfmlDlls | Copy-Item -Destination build -Force
    if (Test-Path build/Debug) {
        $sfmlDlls | Copy-Item -Destination build/Debug -Force
    }
    if (Test-Path build/Release) {
        $sfmlDlls | Copy-Item -Destination build/Release -Force
    }
}

# Copy piece images into the build output so the executable can load them
$pieceSrc = "cpp_chess_engine/piece_pngs"
if (Test-Path $pieceSrc) {
    Copy-Item $pieceSrc build -Recurse -Force
    if (Test-Path build/Debug) {
        Copy-Item $pieceSrc build/Debug -Recurse -Force
    }
    if (Test-Path build/Release) {
        Copy-Item $pieceSrc build/Release -Recurse -Force
    }
}
