#pragma once

#include "interfaces/IScene.hpp"
#include "infrastructure/Database.hpp"
#include <SFML/Graphics.hpp>
#include <string>

// ─────────────────────────────────────────────────────────────────────────────
// HelloScene — Cena de boas-vindas. Exemplo concreto de IScene.
//
// Mostra texto na tela e registra o início da sessão no banco de dados.
// ─────────────────────────────────────────────────────────────────────────────
class HelloScene : public IScene {
public:
    explicit HelloScene(Database& db, const std::string& title);

    void handleEvent(const sf::Event& event) override;
    void update(float deltaTime)             override;
    void render(sf::RenderWindow& window)    override;

private:
    Database&   m_db;
    sf::Font    m_font;
    sf::Text    m_title;
    sf::Text    m_subtitle;
    float       m_elapsed{0.f};
};
