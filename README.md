# GameBase — C++26 + SFML 3 + SQLite + JSON

Projeto base para desenvolvimento de jogos simples.

## Stack

| Componente | Lib |
|---|---|
| Gráficos / Janela | [SFML 3.0](https://sfml-dev.org) via FetchContent |
| JSON | [nlohmann/json 3.11](https://github.com/nlohmann/json) via FetchContent |
| Banco de dados | SQLite3 (sistema) |
| Build | CMake 3.28+ |
| Padrão C++ | C++26 |

## Estrutura

```
src/
  main.cpp                  # Entry point
  core/
    Game.hpp / Game.cpp     # Game loop (SRP)
  interfaces/
    IScene.hpp              # Interface de cena (OCP + LSP)
  scenes/
    HelloScene.hpp/.cpp     # Cena exemplo
  infrastructure/
    Database.hpp/.cpp       # Wrapper SQLite (SRP)
    ConfigManager.hpp       # Leitor de config JSON (SRP)
data/
  config.json               # Configurações da janela e jogo
database.db                 # Criado automaticamente na raiz
```

## Build local (recomendado para desenvolvimento)

```bash
# 1. Instalar dependências de sistema (apenas primeira vez)
bash build.sh deps

# 2. Build debug
bash build.sh

# 3. Executar
./build/GameBase
```

## Build com Docker (opcional)

> Docker + SFML requer X11 forwarding — só funciona em Linux com servidor X.

```bash
# Liberar Docker para usar o display X11
xhost +local:docker

# Build + run
docker compose up --build
```

## Adicionando uma nova cena

1. Crie `src/scenes/MinhaScene.hpp` e `.cpp` herdando `IScene`
2. Implemente `handleEvent`, `update`, `render`
3. Em `main.cpp`: `game.setScene(std::make_unique<MinhaScene>(...));`

O game loop não precisa ser modificado — princípio Open/Closed em ação.
