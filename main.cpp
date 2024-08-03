#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <httplib.h>

using json = nlohmann::json;
using namespace std;

void load_json_config(const std::string& filename) {

    ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening JSON file: " << filename << std::endl;
        return;
    }
    
    json j;
    file >> j;
    std::cout << "[load_json_config] JSON Config:\n" << j.dump(4) << std::endl;
}

void load_yaml_config(const std::string& filename) {

    ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening YAML file: " << filename << std::endl;
        return;
    }
    
    YAML::Node node = YAML::Load(file);
    std::cout << "[load_yaml_config] YAML Config:\n" << node << std::endl;
}

int main() {

    // Objeto JSON simples (nlohmann::json)
    json j;
    j["message"] = "[JSON] Hello, World!";
    j["year"] = 2024;

    // Exibe o JSON no console
    std::cout << j.dump(4) << std::endl;
    
    //

    // Cria um YAML simples
    YAML::Node node;
    node["message"] = "[YAML] Hello, World!";
    node["year"] = 2024;

    // Exibe o YAML no console
    std::cout << "YAML Output:\n" << node << std::endl;

    // Carrega e exibe os arquivos de configuração
    load_json_config("./data/config.json");
    load_yaml_config("./config/config.yml");

    // Usando a biblioteca httplib
    httplib::Client cli("http://httpbin.org");

    auto res = cli.Get("/json");
    if (res && res->status == 200) {
        std::cout << "HTTP GET Response:\n" << res->body << std::endl;
    } else {
        std::cerr << "HTTP GET request failed" << std::endl;
    }

    // Configura o servidor HTTP
    httplib::Server svr;

    svr.Get("/hello", [](const httplib::Request& req, httplib::Response& res){
        res.set_content("Hello, World!", "text/plain");
    });

    // Inicia o servidor HTTP na porta 8083
    std::cout << "Starting server on http://localhost:8083" << std::endl;
    svr.listen("0.0.0.0", 8083);

    return 0;
}