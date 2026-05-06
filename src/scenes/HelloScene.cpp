#include "scenes/HelloScene.hpp"
#include <iostream>
#include <cmath>

HelloScene::HelloScene(Database& db, const std::string& title)
    : m_db(db)
    , m_title(m_font)
    , m_subtitle(m_font)
{
    // Fonte padrão embutida do SFML (sem arquivo externo necessário)
    if (!m_font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf") &&
        !m_font.openFromFile("/usr/share/fonts/TTF/DejaVuSans.ttf") &&
        !m_font.openFromFile("/System/Library/Fonts/Helvetica.ttc"))
    {
        std::cerr << "[HelloScene] Nenhuma fonte padrão encontrada. Texto pode não aparecer.\n";
    }

    m_title.setString(title);
    m_title.setCharacterSize(48);
    m_title.setFillColor(sf::Color::White);
    m_title.setStyle(sf::Text::Bold);

    m_subtitle.setString("Pressione ESC para sair");
    m_subtitle.setCharacterSize(22);
    m_subtitle.setFillColor(sf::Color(200, 200, 200));

    // Registra sessão no banco
    m_db.exec(R"(
        INSERT INTO sessions (scene, started_at)
        VALUES ('HelloScene', datetime('now', 'localtime'));
    )");

    auto rows = m_db.query("SELECT COUNT(*) FROM sessions;");
    if (!rows.empty())
        std::cout << "[HelloScene] Total de sessões registradas: " << rows[0][0] << "\n";
}

void HelloScene::handleEvent(const sf::Event& event) {
    // Tratamento de eventos específicos da cena vai aqui
    (void)event;
}

void HelloScene::update(float deltaTime) {
    m_elapsed += deltaTime;

    // Efeito pulsante no subtítulo
    float alpha = static_cast<float>(155 + 100 * std::sin(m_elapsed * 2.f));
    m_subtitle.setFillColor(sf::Color(200, 200, 200, static_cast<std::uint8_t>(alpha)));
}

void HelloScene::render(sf::RenderWindow& window) {
    auto [w, h] = window.getSize();

    // Centraliza título
    auto tb = m_title.getLocalBounds();
    m_title.setOrigin({tb.position.x + tb.size.x / 2.f, tb.position.y + tb.size.y / 2.f});
    m_title.setPosition({w / 2.f, h / 2.f - 40.f});

    // Centraliza subtítulo
    auto sb = m_subtitle.getLocalBounds();
    m_subtitle.setOrigin({sb.position.x + sb.size.x / 2.f, sb.position.y + sb.size.y / 2.f});
    m_subtitle.setPosition({w / 2.f, h / 2.f + 30.f});

    window.draw(m_title);
    window.draw(m_subtitle);
}
