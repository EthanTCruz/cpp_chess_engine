#!/usr/bin/env bash

if ! command -v cmake >/dev/null 2>&1; then
  echo "Error: cmake is not installed. Please install cmake and try again." >&2
  exit 1
fi

cmake -S . -B build || { echo "CMake configuration failed" >&2; exit 1; }
cmake --build build || { echo "Build failed" >&2; exit 1; }
