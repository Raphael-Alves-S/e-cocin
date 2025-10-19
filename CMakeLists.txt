cmake_minimum_required(VERSION 3.20...4.1)
cmake_policy(VERSION 3.10)  # remove o warning de compat < 3.10

project(e_cocin CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Propaga compatibilidade mínima para subprojetos do FetchContent (CMake 4.x pede isso)
# Você também passará -DCMAKE_POLICY_VERSION_MINIMUM=3.5 na linha de comando (passo 3).
# Aqui garantimos comportamento moderno no topo.
set(CMAKE_WARN_DEPRECATED OFF)

# Traga deps via FetchContent (exceto SQLite)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(cmake/third_party.cmake)

# Encontre o SQLite do MSYS2/MinGW64 (pacote: mingw-w64-x86_64-sqlite3)
find_package(SQLite3 REQUIRED)

# ---- fontes mínimas (placeholder) ----
add_executable(e-cocin
  src/main.cpp
)
target_include_directories(e-cocin PRIVATE src)

target_link_libraries(e-cocin
  PRIVATE
    SQLite::SQLite3
    nlohmann_json::nlohmann_json
    spdlog::spdlog_header_only
)

# ---- testes (opcional) ----
enable_testing()
add_executable(unit_tests tests/test_example.cpp)
target_link_libraries(unit_tests PRIVATE Catch2::Catch2WithMain)
add_test(NAME example_test COMMAND unit_tests)
