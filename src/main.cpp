#include "core/Game.hpp"
#include "infrastructure/ConfigManager.hpp"
#include "infrastructure/Database.hpp"
#include "scenes/HelloScene.hpp"
#include <iostream>

int main() {
    // ── Config ────────────────────────────────────────────────────────────────
    ConfigManager config("data/config.json");

    // ── Banco de dados ────────────────────────────────────────────────────────
    Database db("database.db");
    db.exec(R"(
        CREATE TABLE IF NOT EXISTS sessions (
            id         INTEGER PRIMARY KEY AUTOINCREMENT,
            scene      TEXT    NOT NULL,
            started_at TEXT    NOT NULL
        );
    )");

    // ── Janela / Game Loop ────────────────────────────────────────────────────
    Game::Config gameCfg{
        .title  = config.get<std::string>("window.title",  "GameBase"),
        .width  = config.get<unsigned>   ("window.width",  800),
        .height = config.get<unsigned>   ("window.height", 600),
        .fps    = config.get<unsigned>   ("window.fps",    60),
    };

    Game game(gameCfg);
    game.setScene(std::make_unique<HelloScene>(db, gameCfg.title));
    game.run();

    return 0;
}
