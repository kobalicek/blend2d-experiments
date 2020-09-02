#!/bin/sh

BUILD_DIR="build"
BUILD_OPTIONS="-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"

mkdir -p bllitehtml/${BUILD_DIR}_dbg
mkdir -p bllitehtml/${BUILD_DIR}_rel

eval cmake -B"bllitehtml/${BUILD_DIR}_dbg" -S"bllitehtml" -G"Ninja" -DCMAKE_BUILD_TYPE=Debug ${BUILD_OPTIONS}
eval cmake -B"bllitehtml/${BUILD_DIR}_rel" -S"bllitehtml" -G"Ninja" -DCMAKE_BUILD_TYPE=Release ${BUILD_OPTIONS}
