param(
    [string]$PgnDirectory = 'test_pgns',
    [string]$FenDirectory = 'test_fens',
    [string]$BuildDir = 'build-tests'
)

$ErrorActionPreference = 'Stop'

Write-Host 'Configuring headless test build (GUI disabled, SFML not required)...'
cmake -S . -B $BuildDir -DBUILD_GUI=OFF -DBUILD_TESTING_MODULE=ON

Write-Host 'Building cpp_chess_engine_tests...'
cmake --build $BuildDir --target cpp_chess_engine_tests --config Debug

$exeCandidates = @(
    (Join-Path $BuildDir 'cpp_chess_engine_tests.exe'),
    (Join-Path $BuildDir 'Debug/cpp_chess_engine_tests.exe')
)

$testExe = $exeCandidates | Where-Object { Test-Path $_ } | Select-Object -First 1

if (-not $testExe) {
    throw "Could not locate cpp_chess_engine_tests.exe under '$BuildDir'."
}

if ([System.IO.Path]::IsPathRooted($PgnDirectory)) {
    $resolvedPgnDirectory = $PgnDirectory
} else {
    $resolvedPgnDirectory = Join-Path (Get-Location) $PgnDirectory
}

if ([System.IO.Path]::IsPathRooted($FenDirectory)) {
    $resolvedFenDirectory = $FenDirectory
} else {
    $resolvedFenDirectory = Join-Path (Get-Location) $FenDirectory
}

Write-Host "Running test module: $testExe"
Write-Host "PGN input directory: $resolvedPgnDirectory"
Write-Host "FEN input directory: $resolvedFenDirectory"

& $testExe $resolvedPgnDirectory $resolvedFenDirectory
exit $LASTEXITCODE
