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
