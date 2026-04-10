#!/bin/bash
set -e

CONFIG="${1:-Release}"

echo "========================================"
echo "Building Convoy - $CONFIG"
echo "========================================"

if [ ! -d "build" ]; then
    echo "[CREATE] Creating build directory..."
    mkdir -p build
fi

cd build

if command -v nproc &> /dev/null; then
    CORES=$(nproc)
elif command -v sysctl &> /dev/null; then
    CORES=$(sysctl -n hw.ncpu)
else
    CORES=4
fi

echo "[CONFIGURE] Running CMake..."
cmake .. -DCMAKE_BUILD_TYPE="$CONFIG"

echo "[BUILD] Compiling ($CORES cores)..."
cmake --build . --config "$CONFIG" --parallel "$CORES"

echo ""
echo "========================================"
echo "[SUCCESS] Build complete!"
echo "========================================"
echo ""
echo "Executable: build/Convoy"