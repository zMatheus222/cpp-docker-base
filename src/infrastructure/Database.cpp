#include "Database.hpp"
#include <iostream>

Database::Database(const std::string& path) {
    if (sqlite3_open(path.c_str(), &m_db) != SQLITE_OK) {
        throw std::runtime_error(std::string("Database: falha ao abrir ") + sqlite3_errmsg(m_db));
    }
    std::cout << "[Database] Conectado em: " << path << "\n";
}

Database::~Database() {
    if (m_db) {
        sqlite3_close(m_db);
        std::cout << "[Database] Conexão encerrada.\n";
    }
}

void Database::exec(const std::string& sql) {
    char* errMsg = nullptr;
    if (sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::string err = errMsg ? errMsg : "erro desconhecido";
        sqlite3_free(errMsg);
        throw std::runtime_error("[Database::exec] " + err);
    }
}

Database::RowSet Database::query(const std::string& sql, Callback onRow) {
    RowSet results;

    auto cb = [](void* data, int argc, char** argv, char** /*cols*/) -> int {
        auto& pack = *static_cast<std::pair<RowSet*, Callback>*>(data);
        Row row;
        row.reserve(argc);
        for (int i = 0; i < argc; ++i)
            row.emplace_back(argv[i] ? argv[i] : "NULL");

        pack.first->push_back(row);
        if (pack.second) pack.second(row);
        return 0;
    };

    std::pair<RowSet*, Callback> pack{&results, onRow};
    char* errMsg = nullptr;
    if (sqlite3_exec(m_db, sql.c_str(), cb, &pack, &errMsg) != SQLITE_OK) {
        std::string err = errMsg ? errMsg : "erro desconhecido";
        sqlite3_free(errMsg);
        throw std::runtime_error("[Database::query] " + err);
    }

    return results;
}
