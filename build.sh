#!/usr/bin/env bash
set -e

BUILD_DIR=build

if [ "$1" = "clean" ]; then
    echo "Cleaning..."
    rm -rf build .cache compile_commands.json
    find . -type d -name ".generated" -exec rm -rf {} +
    exit 0
fi

if [ ! -f "$BUILD_DIR/build.ninja" ]; then
    echo "Configuring..."
    rm -rf "$BUILD_DIR"
    CC=clang CXX=clang++ meson setup "$BUILD_DIR"
fi

echo "Building plugin..."
meson compile -C "$BUILD_DIR" libreflect

echo "Building game..."
meson compile -C "$BUILD_DIR"

ln -sf "$BUILD_DIR/compile_commands.json" compile_commands.json

echo "Running..."
"./$BUILD_DIR/an-engine"
