# Define a versão mínima do CMake
cmake_minimum_required(VERSION 3.10)

# Define o nome do projeto e a versão
project(cpp-base VERSION 1.0 LANGUAGES CXX)

# Define o padrão C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Adiciona o suporte para CMake FetchContent
include(FetchContent)

# Baixa a biblioteca nlohmann/json
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3  # Use a versão desejada
)

# Baixa a biblioteca yaml-cpp
FetchContent_Declare(
    yaml_cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG 0.8.0  # Use a versão desejada
)

# Baixa a biblioteca httplib
FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.16.0
)

# Prepara as bibliotecas
FetchContent_MakeAvailable(nlohmann_json yaml_cpp httplib)

# Adiciona o executável e define os arquivos de origem
add_executable(cpp-base main.cpp)

# Linka o executável com as bibliotecas
target_link_libraries(cpp-base PRIVATE nlohmann_json::nlohmann_json yaml-cpp httplib)