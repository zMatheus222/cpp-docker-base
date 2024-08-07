# Escolha uma imagem base com C++ e CMake. Aqui usamos uma imagem oficial do Ubuntu.
FROM ubuntu:22.04

# Instale dependências necessárias (g++, cmake, etc.)
RUN apt update && \
    apt install -y \
    g++ \
    cmake \
    build-essential \
    git \
    && rm -rf /var/lib/apt/lists/*

# Crie um diretório para o projeto
WORKDIR /app

# Copie os arquivos do projeto para o contêiner
COPY . .

# Construa o projeto usando CMake
RUN cmake . && make -j$(nproc)

# Exponha a porta 8083
EXPOSE 8083

# Comando para executar o binário gerado
CMD ["./cpp-base"]
