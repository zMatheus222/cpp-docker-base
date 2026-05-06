FROM ubuntu:24.04

# Dependências do sistema (SFML precisa de libs gráficas mesmo para build)
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    ca-certificates \
    libsqlite3-dev \
    # SFML: dependências de runtime e build
    libx11-dev \
    libxrandr-dev \
    libxcursor-dev \
    libxi-dev \
    libxinerama-dev \
    libudev-dev \
    libfreetype-dev \
    libopenal-dev \
    libflac-dev \
    libvorbis-dev \
    libgl1-mesa-dev \
    libgles2-mesa-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copia todo o projeto
COPY . .

# Build com CMake (SFML e nlohmann/json via FetchContent)
RUN cmake -B build \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    && cmake --build build --parallel "$(nproc)"

CMD ["/app/build/GameBase"]
