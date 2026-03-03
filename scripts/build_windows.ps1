$ErrorActionPreference = 'Stop'

cmake -S . -B build
cmake --build build

function Copy-ToExistingDirs {
    param(
        [Parameter(Mandatory=$true)]
        [string[]]$Destinations,
        [Parameter(Mandatory=$true)]
        [scriptblock]$CopyAction
    )

    foreach ($destination in $Destinations) {
        if (Test-Path $destination) {
            & $CopyAction $destination
        }
    }
}

# Copy SFML runtime libraries next to executable output directories.
$sfmlRoot = "build/_deps/sfml-build"
$sfmlBin = Join-Path $sfmlRoot "bin"
$sfmlLib = Join-Path $sfmlRoot "lib"
$sfmlDlls = @()

if (Test-Path $sfmlBin) {
    $sfmlDlls = Get-ChildItem -Path $sfmlBin -Filter '*.dll' -Recurse
}

if ($sfmlDlls.Count -eq 0 -and (Test-Path $sfmlLib)) {
    $sfmlDlls = Get-ChildItem -Path $sfmlLib -Filter '*.dll' -Recurse
}

$outputDirs = @(
    'build',
    'build/Debug',
    'build/Release',
    'out/build/x64-Debug',
    'out/build/x64-Release',
    'x64/Debug',
    'x64/Release'
)

if ($sfmlDlls.Count -gt 0) {
    Copy-ToExistingDirs -Destinations $outputDirs -CopyAction {
        param($destination)
        $sfmlDlls | Copy-Item -Destination $destination -Force
    }
} else {
    Write-Warning "No SFML DLLs were found under '$sfmlBin' or '$sfmlLib'."
}

# Copy piece images into executable output directories.
$pieceSrc = "cpp_chess_engine/piece_pngs"
if (Test-Path $pieceSrc) {
    Copy-ToExistingDirs -Destinations $outputDirs -CopyAction {
        param($destination)
        Copy-Item $pieceSrc $destination -Recurse -Force
    }
} else {
    Write-Warning "Piece image directory '$pieceSrc' was not found."
}
