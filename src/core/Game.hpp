#pragma once

#include "interfaces/IScene.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

// ─────────────────────────────────────────────────────────────────────────────
// Game — Gerencia a janela SFML e o game loop principal.
//
// Princípio:  Single Responsibility
//   Responsável apenas por: janela, loop, tempo e delegação para IScene.
// ─────────────────────────────────────────────────────────────────────────────
class Game {
public:
    struct Config {
        std::string title  = "GameBase";
        unsigned    width  = 800;
        unsigned    height = 600;
        unsigned    fps    = 60;
    };

    explicit Game(Config cfg);

    // Define a cena ativa (troca de cena via polimorfismo)
    void setScene(std::unique_ptr<IScene> scene);

    // Inicia o loop; bloqueia até a janela fechar
    void run();

private:
    Config               m_cfg;
    sf::RenderWindow     m_window;
    std::unique_ptr<IScene> m_scene;
};
