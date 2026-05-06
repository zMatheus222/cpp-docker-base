#pragma once

#include <SFML/Graphics.hpp>

// ─────────────────────────────────────────────────────────────────────────────
// IScene — Interface base para todas as cenas do jogo.
//
// Princípio:  Open/Closed + Liskov Substitution
//   Novas cenas herdam IScene sem modificar o Game loop.
// ─────────────────────────────────────────────────────────────────────────────
class IScene {
public:
    virtual ~IScene() = default;

    virtual void handleEvent(const sf::Event& event) = 0;
    virtual void update(float deltaTime)              = 0;
    virtual void render(sf::RenderWindow& window)     = 0;
};
