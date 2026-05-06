#pragma once

#include <nlohmann/json.hpp>
#include <fstream>
#include <stdexcept>
#include <string>

// ─────────────────────────────────────────────────────────────────────────────
// ConfigManager — Lê e disponibiliza configurações a partir de um JSON.
//
// Princípio:  Single Responsibility
//   Única responsabilidade: carregar e expor config em JSON.
// ─────────────────────────────────────────────────────────────────────────────
class ConfigManager {
public:
    explicit ConfigManager(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open())
            throw std::runtime_error("ConfigManager: não foi possível abrir " + path);
        file >> m_data;
    }

    // Acessa um valor com fallback, ex: get("window.width", 800)
    template <typename T>
    T get(const std::string& key, T fallback = T{}) const {
        try {
            // Suporte a chaves aninhadas separadas por ponto
            auto node  = &m_data;
            std::string remaining = key;
            while (true) {
                auto dot = remaining.find('.');
                auto part = (dot == std::string::npos) ? remaining : remaining.substr(0, dot);
                node = &(*node).at(part);
                if (dot == std::string::npos) break;
                remaining = remaining.substr(dot + 1);
            }
            return node->get<T>();
        } catch (...) {
            return fallback;
        }
    }

    const nlohmann::json& raw() const { return m_data; }

private:
    nlohmann::json m_data;
};
