#include "core/Game.hpp"
#include <iostream>

Game::Game(Config cfg)
    : m_cfg(std::move(cfg))
    , m_window(
        sf::VideoMode({m_cfg.width, m_cfg.height}),
        m_cfg.title,
        sf::Style::Default
    )
{
    m_window.setFramerateLimit(m_cfg.fps);
    std::cout << "[Game] Janela criada: "
              << m_cfg.width << "x" << m_cfg.height
              << " @ " << m_cfg.fps << "fps\n";
}

void Game::setScene(std::unique_ptr<IScene> scene) {
    m_scene = std::move(scene);
}

void Game::run() {
    if (!m_scene) {
        std::cerr << "[Game] Nenhuma cena definida. Encerrando.\n";
        return;
    }

    sf::Clock clock;

    while (m_window.isOpen()) {
        float dt = clock.restart().asSeconds();

        // ── Eventos ──────────────────────────────────────────────────────────
        while (const auto event = m_window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                m_window.close();

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
                if (key->code == sf::Keyboard::Key::Escape)
                    m_window.close();

            if (m_scene) m_scene->handleEvent(*event);
        }

        // ── Update ───────────────────────────────────────────────────────────
        if (m_scene) m_scene->update(dt);

        // ── Render ───────────────────────────────────────────────────────────
        m_window.clear(sf::Color(30, 30, 46)); // fundo escuro
        if (m_scene) m_scene->render(m_window);
        m_window.display();
    }

    std::cout << "[Game] Loop encerrado.\n";
}
