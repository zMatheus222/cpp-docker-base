#!/usr/bin/env bash
set -euo pipefail

# Instala dependências de sistema (Ubuntu/Debian)
install_deps() {
    echo "→ Instalando dependências..."
    sudo apt-get update && sudo apt-get install -y \
        build-essential cmake git libsqlite3-dev \
        libx11-dev libxrandr-dev libxcursor-dev libxi-dev libxinerama-dev \
        libudev-dev libfreetype-dev libopenal-dev libflac-dev libvorbis-dev \
        libgl1-mesa-dev fonts-dejavu
}

BUILD_TYPE="${1:-Debug}"

if [[ "${1:-}" == "deps" ]]; then
    install_deps
    exit 0
fi

echo "→ Configurando CMake (${BUILD_TYPE})..."
cmake -B build -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"

echo "→ Compilando..."
cmake --build build --parallel "$(nproc)"

echo ""
echo "✓ Build concluído: ./build/GameBase"
echo "  Execute: ./build/GameBase"
