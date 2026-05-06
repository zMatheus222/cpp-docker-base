#pragma once

#include <sqlite3.h>
#include <string>
#include <vector>
#include <functional>
#include <stdexcept>

// ─────────────────────────────────────────────────────────────────────────────
// Database — Wrapper RAII em torno do SQLite3.
//
// Princípio:  Single Responsibility
//   Lida exclusivamente com persistência via SQLite.
// ─────────────────────────────────────────────────────────────────────────────
class Database {
public:
    using Row      = std::vector<std::string>;
    using RowSet   = std::vector<Row>;
    using Callback = std::function<void(const Row&)>;

    explicit Database(const std::string& path);
    ~Database();

    // Não copiável — RAII ownership único
    Database(const Database&)            = delete;
    Database& operator=(const Database&) = delete;

    // Executa SQL sem retorno (CREATE, INSERT, UPDATE, DELETE)
    void exec(const std::string& sql);

    // Executa SQL com retorno de linhas; opcional: callback por linha
    RowSet query(const std::string& sql, Callback onRow = nullptr);

private:
    sqlite3* m_db = nullptr;

    static int staticCallback(void* data, int argc, char** argv, char** /*colNames*/);
};
