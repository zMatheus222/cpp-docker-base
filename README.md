# GameBase — C++26 + SFML 3 + SQLite + JSON

Projeto base para desenvolvimento de jogos 2D simples do zero.  
Este documento explica cada arquivo, cada decisão de design e como estender o projeto.

---

## Sumário

1. [Como rodar](#como-rodar)
2. [Estrutura do projeto](#estrutura-do-projeto)
3. [CMakeLists.txt — o sistema de build](#cmakeliststxt)
4. [src/main.cpp — o ponto de entrada](#srcmaincpp)
5. [src/interfaces/IScene.hpp — contrato das cenas](#srcinterfacesiscenehpp)
6. [src/core/Game — o game loop](#srccorgame)
7. [src/infrastructure/ConfigManager.hpp — configuração JSON](#srcinfrastructureconfigmanagerhpp)
8. [src/infrastructure/Database — persistência SQLite](#srcinfrastructuredatabase)
9. [src/scenes/HelloScene — a primeira cena](#srcsceneshelloscene)
10. [data/config.json — configuração da janela](#dataconfigjson)
11. [Princípios SOLID aplicados](#princípios-solid-aplicados)
12. [Tutorial: criando uma nova cena](#tutorial-criando-uma-nova-cena)
13. [Tutorial: adicionando uma nova tabela no banco](#tutorial-adicionando-uma-nova-tabela-no-banco)
14. [Tutorial: lendo valores do config.json](#tutorial-lendo-valores-do-configjson)

---

## Como rodar

### Windows 11 com WSL2 (recomendado)

```bash
# Antes de rodar pela primeira vez — libera o display gráfico
export DISPLAY=:0

# Instala dependências de sistema (apenas uma vez)
bash build.sh deps

# Compila o projeto
bash build.sh

# Executa — a janela SFML abre no Windows via WSLg
./build/GameBase
```

> **Dica:** adicione `export DISPLAY=:0` no final do seu `~/.bashrc` para não
> precisar repetir toda vez que abrir o terminal.

### Linux nativo / macOS

```bash
bash build.sh deps
bash build.sh
./build/GameBase
```

---

## Estrutura do projeto

```
game-base/
├── CMakeLists.txt                  # Sistema de build (CMake)
├── build.sh                        # Script de conveniência para Linux/WSL
├── build.ps1                       # Script de conveniência para Windows (MSYS2)
├── Dockerfile                      # Build em container (opcional)
├── docker-compose.yml              # Orquestração Docker (opcional)
│
├── data/
│   └── config.json                 # Configurações da janela e do jogo
│
├── database.db                     # Criado automaticamente ao rodar
│
└── src/
    ├── main.cpp                    # Ponto de entrada — conecta tudo
    │
    ├── interfaces/
    │   └── IScene.hpp              # Contrato que toda cena deve seguir
    │
    ├── core/
    │   ├── Game.hpp                # Declaração da classe Game
    │   └── Game.cpp                # Game loop: eventos → update → render
    │
    ├── infrastructure/
    │   ├── ConfigManager.hpp       # Lê config.json (header-only)
    │   ├── Database.hpp            # Wrapper SQLite — declaração
    │   └── Database.cpp            # Wrapper SQLite — implementação
    │
    └── scenes/
        ├── HelloScene.hpp          # Cena de exemplo — declaração
        └── HelloScene.cpp          # Cena de exemplo — implementação
```

**Por que esta estrutura?**
Ela separa responsabilidades em camadas:
- `interfaces/` → contratos abstratos (o "o quê")
- `core/` → motor do jogo (o "como" do loop)
- `infrastructure/` → serviços de suporte (banco, config)
- `scenes/` → conteúdo do jogo (o que acontece na tela)

---

## CMakeLists.txt

O CMake é o sistema de build. Ele descreve *como* compilar o projeto sem depender
de um IDE específico.

```cmake
cmake_minimum_required(VERSION 3.28)
project(GameBase VERSION 1.0.0 LANGUAGES CXX)
```

Define o nome do projeto e as linguagens usadas (`CXX` = C++; `C` é adicionado
automaticamente pelo SQLite).

```cmake
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
```

- `CXX_STANDARD 26` — usa C++26
- `STANDARD_REQUIRED ON` — falha o build se o compilador não suportar
- `EXTENSIONS OFF` — desativa extensões proprietárias do GCC/Clang (código mais portável)

### FetchContent — dependências automáticas

```cmake
include(FetchContent)

FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        3.0.0
    GIT_SHALLOW    TRUE   # baixa apenas o commit da tag, não o histórico inteiro
)
FetchContent_MakeAvailable(SFML)
```

`FetchContent` baixa e compila dependências automaticamente no primeiro build.
Depois ficam em cache em `build/_deps/`. Você nunca precisa instalar SFML
manualmente. O mesmo padrão é usado para `nlohmann_json` e para o SQLite3
(que vem como um único arquivo `.c` — a "amalgamation" oficial).

### Targets e linking

```cmake
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    SFML::Graphics   # janela, texturas, sprites, formas
    SFML::Window     # eventos de teclado/mouse
    SFML::System     # Clock, Time, Vector
    SFML::Audio      # sons e música
    nlohmann_json::nlohmann_json
    sqlite3_lib
)
```

`PRIVATE` significa que essas libs são internas ao executável — outros projetos
que dependessem do GameBase não herdariam essas dependências.

### Cópia automática de assets

```cmake
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/data
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/data
)
```

Após cada compilação, a pasta `data/` é copiada para junto do executável.
Isso garante que `config.json` sempre esteja no lugar certo quando o programa rodar.

---

## src/main.cpp

O ponto de entrada do programa. Responsabilidade única: **conectar as peças**.

```cpp
int main() {
    // 1. Carrega configuração do JSON
    ConfigManager config("data/config.json");

    // 2. Abre o banco de dados e cria tabelas necessárias
    Database db("database.db");
    db.exec(R"(
        CREATE TABLE IF NOT EXISTS sessions ( ... );
    )");

    // 3. Monta a configuração da janela com valores do JSON
    Game::Config gameCfg{
        .title  = config.get<std::string>("window.title",  "GameBase"),
        .width  = config.get<unsigned>   ("window.width",  800),
        .height = config.get<unsigned>   ("window.height", 600),
        .fps    = config.get<unsigned>   ("window.fps",    60),
    };

    // 4. Cria o jogo, define a cena inicial e entra no loop
    Game game(gameCfg);
    game.setScene(std::make_unique<HelloScene>(db, gameCfg.title));
    game.run(); // bloqueia até a janela fechar
}
```

**Padrão usado: Composition Root** — todos os objetos são criados e conectados
em um único lugar. Isso facilita substituir dependências (ex: trocar `Database`
por um mock para testes).

**`std::make_unique`** cria um ponteiro inteligente. Quando o `game` for destruído,
a cena é destruída automaticamente. Sem `delete` manual, sem memory leak.

---

## src/interfaces/IScene.hpp

```cpp
class IScene {
public:
    virtual ~IScene() = default;

    virtual void handleEvent(const sf::Event& event) = 0;
    virtual void update(float deltaTime)              = 0;
    virtual void render(sf::RenderWindow& window)     = 0;
};
```

Uma *interface* em C++ é uma classe com todos os métodos `= 0` (puramente virtuais).
Ela define um **contrato**: qualquer classe que herdar `IScene` é obrigada a
implementar os três métodos.

**Por que separar em três métodos?**

| Método | Quando é chamado | O que faz |
|---|---|---|
| `handleEvent` | Para cada evento da fila | Teclado, mouse, fechar janela |
| `update` | Uma vez por frame | Move objetos, aplica física, atualiza lógica |
| `render` | Após o update | Desenha tudo na tela |

Esta separação é o núcleo de todo game loop clássico.

**`virtual ~IScene() = default`** — o destrutor virtual é obrigatório em interfaces.
Sem ele, destruir um objeto do tipo `IScene*` que aponta para `HelloScene`
não chamaria o destrutor correto (undefined behavior).

---

## src/core/Game

### Game.hpp — declaração

```cpp
class Game {
public:
    struct Config {
        std::string title  = "GameBase";
        unsigned    width  = 800;
        unsigned    height = 600;
        unsigned    fps    = 60;
    };

    explicit Game(Config cfg);
    void setScene(std::unique_ptr<IScene> scene);
    void run();

private:
    Config                  m_cfg;
    sf::RenderWindow        m_window;
    std::unique_ptr<IScene> m_scene;
};
```

`struct Config` aninhada agrupa os parâmetros de criação. É mais legível do que
um construtor com quatro argumentos soltos.

`std::unique_ptr<IScene>` guarda a cena atual com *ownership* exclusivo.
Quando `setScene` é chamado com uma nova cena, a anterior é destruída automaticamente.

### Game.cpp — o game loop

```cpp
void Game::run() {
    sf::Clock clock;

    while (m_window.isOpen()) {
        float dt = clock.restart().asSeconds(); // tempo do último frame em segundos

        // 1. Eventos
        while (const auto event = m_window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                m_window.close();
            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
                if (key->code == sf::Keyboard::Key::Escape)
                    m_window.close();
            if (m_scene) m_scene->handleEvent(*event);
        }

        // 2. Update
        if (m_scene) m_scene->update(dt);

        // 3. Render
        m_window.clear(sf::Color(30, 30, 46));
        if (m_scene) m_scene->render(m_window);
        m_window.display();
    }
}
```

**Delta time (`dt`):** a cada frame, medimos quantos segundos se passaram desde
o frame anterior. Multiplicar velocidades por `dt` garante que o jogo rode na
mesma velocidade independente do FPS da máquina:

```cpp
// Errado — depende do FPS: em 30fps move 5px, em 60fps move 5px por frame (o dobro)
posicao.x += 5.0f;

// Certo — sempre 300 pixels por segundo, qualquer que seja o FPS
posicao.x += 300.0f * dt;
```

**`pollEvent` no SFML 3** retorna um `std::optional<sf::Event>`. O loop `while`
consome eventos até a fila esvaziar. `getIf<T>()` retorna um ponteiro para o
tipo concreto do evento, ou `nullptr` se for outro tipo.

---

## src/infrastructure/ConfigManager.hpp

Header-only (toda a implementação fica no `.hpp`). Lê um arquivo JSON e expõe
os valores com um método `get<T>`.

```cpp
ConfigManager config("data/config.json");

std::string titulo = config.get<std::string>("window.title", "GameBase");
unsigned    largura = config.get<unsigned>("window.width", 800);
bool        debug   = config.get<bool>("game.debug", false);
```

O segundo argumento é o **valor padrão** — retornado se a chave não existir
no JSON. Isso evita crashes se o arquivo de config estiver incompleto.

**Chaves aninhadas com ponto:** `"window.title"` acessa `config["window"]["title"]`.
O método percorre o JSON recursivamente dividindo a string pelo `.`:

```cpp
template <typename T>
T get(const std::string& key, T fallback = T{}) const {
    try {
        auto node = &m_data;
        std::string remaining = key;
        while (true) {
            auto dot  = remaining.find('.');
            auto part = (dot == std::string::npos) ? remaining : remaining.substr(0, dot);
            node = &(*node).at(part);
            if (dot == std::string::npos) break;
            remaining = remaining.substr(dot + 1);
        }
        return node->get<T>();
    } catch (...) {
        return fallback; // qualquer erro → retorna o fallback
    }
}
```

---

## src/infrastructure/Database

Wrapper RAII em torno da API C do SQLite3.

**RAII** (Resource Acquisition Is Initialization): o recurso (conexão com o banco)
é adquirido no construtor e liberado no destrutor. Você nunca precisa chamar
`sqlite3_close` manualmente.

```cpp
// Construtor — abre a conexão
Database::Database(const std::string& path) {
    if (sqlite3_open(path.c_str(), &m_db) != SQLITE_OK)
        throw std::runtime_error(...);
}

// Destrutor — fecha automaticamente quando o objeto sai de escopo
Database::~Database() {
    if (m_db) sqlite3_close(m_db);
}
```

### Métodos disponíveis

```cpp
// exec — comandos sem retorno (CREATE, INSERT, UPDATE, DELETE)
db.exec("INSERT INTO sessions (scene, started_at) VALUES ('Menu', datetime('now'))");

// query — SELECT, retorna vector de linhas (cada linha é um vector<string>)
auto rows = db.query("SELECT id, scene FROM sessions;");
for (const auto& row : rows) {
    std::cout << row[0] << " | " << row[1] << "\n";
}

// query com callback — processa linha a linha sem guardar tudo em memória
db.query("SELECT * FROM sessions;", [](const Database::Row& row) {
    std::cout << "Cena: " << row[1] << "\n";
});
```

### Raw strings para SQL

```cpp
db.exec(R"(
    CREATE TABLE IF NOT EXISTS scores (
        id         INTEGER PRIMARY KEY AUTOINCREMENT,
        player     TEXT    NOT NULL,
        score      INTEGER NOT NULL,
        created_at TEXT    NOT NULL
    );
)");
```

`R"(...)"` é um *raw string literal* — não precisa escapar aspas ou quebras de linha.
Ideal para blocos SQL com múltiplas linhas.

---

## src/scenes/HelloScene

Demonstra como uma cena concreta implementa `IScene`.

### HelloScene.hpp

```cpp
class HelloScene : public IScene {
public:
    explicit HelloScene(Database& db, const std::string& title);

    void handleEvent(const sf::Event& event) override;
    void update(float deltaTime)             override;
    void render(sf::RenderWindow& window)    override;

private:
    Database&  m_db;      // referência — não possui o banco, só usa
    sf::Font   m_font;
    sf::Text   m_title;
    sf::Text   m_subtitle;
    float      m_elapsed{0.f};
};
```

`Database& m_db` é uma referência, não um ponteiro nem um valor. Isso significa:
- A cena não é dona do banco (quem cria é o `main.cpp`)
- Não pode ser `nullptr`
- Não precisa de `delete`

### Construtor

```cpp
HelloScene::HelloScene(Database& db, const std::string& title)
    : m_db(db)
    , m_title(m_font)      // SFML 3: Text recebe Font no construtor
    , m_subtitle(m_font)
{
    // Busca a primeira fonte disponível no sistema
    const std::vector<std::string> fontPaths = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",  // Linux
        "C:/Windows/Fonts/arial.ttf",                        // Windows
        "/System/Library/Fonts/Helvetica.ttc",               // macOS
        // ...
    };
    for (const auto& path : fontPaths)
        if (m_font.openFromFile(path)) break;

    m_title.setString(title);
    m_title.setCharacterSize(48);
    m_title.setFillColor(sf::Color::White);

    // Registra a sessão no banco de dados
    m_db.exec(R"(INSERT INTO sessions (scene, started_at)
                 VALUES ('HelloScene', datetime('now', 'localtime'));)");
}
```

### update — animação com delta time

```cpp
void HelloScene::update(float deltaTime) {
    m_elapsed += deltaTime;

    // sin() varia de -1 a 1 → alpha varia de 55 a 255
    float alpha = 155 + 100 * std::sin(m_elapsed * 2.f);
    m_subtitle.setFillColor(sf::Color(200, 200, 200, static_cast<uint8_t>(alpha)));
}
```

`m_elapsed` acumula o tempo total e `sin` cria uma oscilação suave no canal alpha
do texto — efeito de "pulsação" completamente independente do FPS.

### render — centralização dinâmica

```cpp
void HelloScene::render(sf::RenderWindow& window) {
    auto [w, h] = window.getSize(); // structured binding (C++17+)

    auto tb = m_title.getLocalBounds();
    m_title.setOrigin({tb.position.x + tb.size.x / 2.f,
                       tb.position.y + tb.size.y / 2.f});
    m_title.setPosition({w / 2.f, h / 2.f - 40.f});

    window.draw(m_title);
    window.draw(m_subtitle);
}
```

`getLocalBounds()` retorna o bounding box do texto. Definir a origem no centro
e posicionar no centro da janela garante centralização correta para qualquer
string, independente do tamanho.

---

## data/config.json

```json
{
  "window": {
    "title":  "GameBase",
    "width":  800,
    "height": 600,
    "fps":    60
  },
  "game": {
    "version": "1.0.0",
    "debug":   true
  }
}
```

Qualquer valor daqui pode ser lido com `config.get<T>("chave", fallback)`.
Para adicionar novas configurações (volume, velocidade, etc.), basta adicionar
a chave aqui e ler onde precisar — sem recompilar.

---

## Princípios SOLID aplicados

| Princípio | Como aparece no projeto |
|---|---|
| **S** Single Responsibility | `Game` só cuida do loop. `Database` só cuida do SQLite. `ConfigManager` só lê JSON. |
| **O** Open/Closed | Para adicionar cenas, herda-se `IScene`. O `Game` não muda. |
| **L** Liskov Substitution | Qualquer `IScene` pode ser passada para `game.setScene()` sem quebrar o loop. |
| **I** Interface Segregation | `IScene` tem apenas o que toda cena precisa: evento, update, render. |
| **D** Dependency Inversion | `Game` depende de `IScene` (abstração), não de `HelloScene` (concreção). |

---

## Tutorial: criando uma nova cena

Exemplo: uma tela de menu com duas opções.

**1. Crie `src/scenes/MenuScene.hpp`:**

```cpp
#pragma once
#include "interfaces/IScene.hpp"
#include <SFML/Graphics.hpp>

class MenuScene : public IScene {
public:
    explicit MenuScene(sf::Font& font);

    void handleEvent(const sf::Event& event) override;
    void update(float deltaTime)             override;
    void render(sf::RenderWindow& window)    override;

private:
    sf::Text m_title;
    sf::Text m_opcao1;
    sf::Text m_opcao2;
};
```

**2. Crie `src/scenes/MenuScene.cpp`:**

```cpp
#include "scenes/MenuScene.hpp"

MenuScene::MenuScene(sf::Font& font)
    : m_title(font), m_opcao1(font), m_opcao2(font)
{
    m_title.setString("Menu Principal");
    m_title.setCharacterSize(48);

    m_opcao1.setString("Jogar");
    m_opcao1.setCharacterSize(28);

    m_opcao2.setString("Sair");
    m_opcao2.setCharacterSize(28);
}

void MenuScene::handleEvent(const sf::Event& event) {
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Enter) {
            // trocar de cena aqui
        }
    }
}

void MenuScene::update(float /*dt*/) { }

void MenuScene::render(sf::RenderWindow& window) {
    window.draw(m_title);
    window.draw(m_opcao1);
    window.draw(m_opcao2);
}
```

**3. Em `main.cpp`, troque a cena:**

```cpp
#include "scenes/MenuScene.hpp"

// ...
game.setScene(std::make_unique<MenuScene>(minhaFont));
```

O `Game::run()` não precisa de nenhuma modificação — Open/Closed em ação.

---

## Tutorial: adicionando uma nova tabela no banco

Em `main.cpp`, adicione o `CREATE TABLE` junto com os outros:

```cpp
db.exec(R"(
    CREATE TABLE IF NOT EXISTS high_scores (
        id         INTEGER PRIMARY KEY AUTOINCREMENT,
        player     TEXT    NOT NULL,
        score      INTEGER NOT NULL,
        created_at TEXT    NOT NULL DEFAULT (datetime('now', 'localtime'))
    );
)");
```

Para inserir um score numa cena que recebe `Database& m_db`:

```cpp
m_db.exec("INSERT INTO high_scores (player, score) VALUES ('Matheus', 1500);");
```

Para ler os 5 maiores scores:

```cpp
auto rows = m_db.query(
    "SELECT player, score FROM high_scores ORDER BY score DESC LIMIT 5;"
);
for (const auto& row : rows) {
    std::cout << row[0] << ": " << row[1] << "\n";
}
```

---

## Tutorial: lendo valores do config.json

**1. Adicione a chave no JSON:**

```json
{
  "window": { ... },
  "game": {
    "version": "1.0.0",
    "debug": true,
    "player_speed": 250.0,
    "starting_lives": 3
  }
}
```

**2. Leia no código:**

```cpp
float velocidade = config.get<float>("game.player_speed", 200.0f);
int   vidas      = config.get<int>("game.starting_lives", 3);
bool  debug      = config.get<bool>("game.debug", false);
```

O valor padrão (segundo argumento) é usado se a chave não existir — útil
durante desenvolvimento quando o config ainda está incompleto, ou quando
você distribui o jogo e o jogador apaga ou edita o arquivo.

