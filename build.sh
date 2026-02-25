#!/usr/bin/env bash

# Hentikan script jika ada error
set -e

# Warna untuk output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== Memulai Proses Build InterpKit ===${NC}"

# ==========================================
# 1. Build untuk Linux (Native)
# ==========================================
echo -e "\n${GREEN}[1/2] Membangun versi Linux...${NC}"

# Tentukan command cmake yang akan dipakai
CMAKE_CMD="cmake"
if [ -x "/tmp/cmake-venv/bin/cmake" ]; then
    CMAKE_CMD="/tmp/cmake-venv/bin/cmake"
fi

$CMAKE_CMD -B build -DCMAKE_BUILD_TYPE=Release
$CMAKE_CMD --build build -j$(nproc)

echo -e "${GREEN}--> Build Linux selesai! Executable: build/interpkit${NC}"

# ==========================================
# 2. Build untuk Windows (Cross-compile)
# ==========================================
echo -e "\n${GREEN}[2/2] Membangun versi Windows (.exe)...${NC}"

if command -v x86_64-w64-mingw32-g++ &> /dev/null; then
    $CMAKE_CMD -B build-win -DCMAKE_TOOLCHAIN_FILE=cmake/mingw-w64-x86_64.cmake -DCMAKE_BUILD_TYPE=Release
    $CMAKE_CMD --build build-win -j$(nproc)
    echo -e "${GREEN}--> Build Windows selesai! Executable: build-win/interpkit.exe${NC}"
else
    echo -e "\n⚠️  Kompiler MinGW-w64 (x86_64-w64-mingw32-g++) tidak ditemukan."
    echo "Lewati build Windows. Silakan install mingw-w64 jika ingin mem-build versi .exe."
fi

echo -e "\n${BLUE}=== Semua proses selesai! ===${NC}"
