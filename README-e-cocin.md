
# e-cocin — Setup (Windows + MSYS2/MinGW64) e Build

Este guia explica **exatamente** como reproduzir o ambiente que usamos:

- **MinGW64 (MSYS2)** como toolchain (GCC 15+)
- **CMake 4.x**
- Dependências via **FetchContent** (nlohmann/json, spdlog, Catch2)
- **SQLite** do MSYS2 (via `find_package(SQLite3)`)

> Estrutura que estou assumindo no repo:
>
> ```
> e-cocin/
> ├─ .vscode/
> ├─ cmake/
> │  └─ third_party.cmake
> ├─ src/
> │  └─ main.cpp
> ├─ tests/
> │  └─ test_example.cpp
> └─ CMakeLists.txt
> ```
>
> **Não** comite a pasta `build/`.

---

## 1) Instalar o MSYS2/MinGW64

### 1.1 Instalar MSYS2 (via winget)
Abra **PowerShell** e execute:
```powershell
winget install -e --id MSYS2.MSYS2
```

### 1.2 Abrir o terminal correto
- Menu Iniciar → **MSYS2 MinGW 64-bit**  
  O prompt deve mostrar algo como `MINGW64 ~`.

### 1.3 Atualizar e instalar ferramentas
```bash
pacman -Syu
# Feche o terminal e abra de novo, depois repita:
pacman -Syu

# Instalar toolchain e utilitários
pacman -S --needed base-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake git

# (opcional) Ninja para builds rápidos
pacman -S --needed mingw-w64-x86_64-ninja

# SQLite (pacote oficial do MSYS2)
pacman -S --needed mingw-w64-x86_64-sqlite3
```

### 1.4 Verificar versões
```bash
g++ --version
cmake --version
```
Saída esperada:
```
g++.exe (Rev..., Built by MSYS2 project) 15.2.0
cmake version 4.1.2
```

---

## 2) Clonar e entrar no projeto

```bash
cd /SeuDiretorio/e-cocin
```

---

## 3) CMakeLists.txt (resumo)

```cmake
cmake_minimum_required(VERSION 3.20...4.1)
cmake_policy(VERSION 3.10)

project(e_cocin CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(cmake/third_party.cmake)

find_package(SQLite3 REQUIRED)

add_executable(ecommerce src/main.cpp)

target_include_directories(ecommerce PRIVATE src)

target_link_libraries(ecommerce
  PRIVATE
    SQLite::SQLite3
    nlohmann_json::nlohmann_json
    spdlog::spdlog_header_only
)

enable_testing()
add_executable(unit_tests tests/test_example.cpp)
target_link_libraries(unit_tests PRIVATE Catch2::Catch2WithMain)
add_test(NAME example_test COMMAND unit_tests)
```

---

## 4) third_party.cmake

```cmake
include(FetchContent)

FetchContent_Declare(nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.13.0
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.2
)
FetchContent_MakeAvailable(catch2)
```

---

## 5) Código mínimo

**src/main.cpp**
```cpp
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <sqlite3.h>
#include <iostream>

int main() {
  nlohmann::json j = { {"hello", "world"}, {"answer", 42} };
  spdlog::info("JSON: {}", j.dump());

  sqlite3* db = nullptr;
  if (sqlite3_open(":memory:", &db) == SQLITE_OK) {
    spdlog::info("SQLite in-memory OK");
    sqlite3_close(db);
  } else {
    spdlog::error("Falha ao abrir SQLite");
  }

  std::cout << "e-cocin build OK\n";
  return 0;
}
```

**tests/test_example.cpp**
```cpp
#include <catch2/catch_all.hpp>
TEST_CASE("smoke") { REQUIRE(2 + 2 == 4); }
```

---

## 6) Build e execução

```bash
rm -rf build

# Configurar (inclui policy mínima 3.5 para deps antigas)
cmake -G "MinGW Makefiles" -B build -S . -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Compilar
cmake --build build -j

# Executar
./build/e-cocin.exe

# Testes (opcional)
ctest --test-dir build --output-on-failure
```

---

## 7) VS Code (IntelliSense)

`.vscode/settings.json`
```json
{
  "C_Cpp.default.compilerPath": "C:/msys64/mingw64/bin/g++.exe",
  "C_Cpp.default.intelliSenseMode": "windows-gcc-x64",
  "C_Cpp.default.compileCommands": "${workspaceFolder}/build/compile_commands.json",
  "cmake.generator": "MinGW Makefiles"
}
```

Depois:
1. **Ctrl+Shift+P → CMake: Select a Kit** → escolha **GCC (MSYS2 MinGW 64-bit)**.
2. **Ctrl+Shift+P → CMake: Configure**.
3. **Ctrl+Shift+P → CMake: Build**.

---

## 8) Problemas comuns

| Erro | Solução |
|------|----------|
| `Compatibility with CMake < 3.5...` | Adicione `-DCMAKE_POLICY_VERSION_MINIMUM=3.5` no configure |
| `find_package(SQLite3)` não encontra | Instale com `pacman -S mingw-w64-x86_64-sqlite3` |
| Includes sublinhados no VS Code | Use `CMAKE_EXPORT_COMPILE_COMMANDS=ON` e ajuste `.vscode/settings.json` |
| `URL_HASH SHA256=SKIP` | Forneça hash real ou use o SQLite do MSYS2 (como aqui) |

---

## 9) Próximos passos

- Adicionar `oatpp` em `third_party.cmake` para criar a API REST.  
- Criar entidades e camadas (domain, repository, services, controllers).

---

## 10) Confirmação do build bem-sucedido

Saída esperada:

```
[info] JSON: {"answer":42,"hello":"world"}
[info] SQLite in-memory OK
e-cocin build OK
1/1 Test #1: example_test ..........   Passed
```

Tudo certo 🎉


por que usamos virtual nos métodos do repositório?
→ porque o repositório é uma interface, não uma implementação.

Em C++, quando você quer declarar uma interface (como em Java), você faz isso usando métodos virtuais puros (= 0) dentro de uma classe-base.

🧱 exemplo prático (seu caso)
class IClientRepository {
public:
    virtual ~IClientRepository() = default;

    virtual Client create(const Client& in) = 0;
    virtual std::optional<Client> findById(long long id) = 0;
    virtual std::vector<Client> listAll() = 0;
    virtual bool update(const Client& c) = 0;
    virtual bool remove(long long id) = 0;
};


➡️ Aqui, IClientRepository é abstrata — você não pode instanciá-la diretamente.

Depois, uma classe concreta implementa essa interface:

class ClientRepositorySqlite : public IClientRepository {
public:
    Client create(const Client& in) override;
    std::optional<Client> findById(long long id) override;
    std::vector<Client> listAll() override;
    bool update(const Client& c) override;
    bool remove(long long id) override;
};

⚙️ o que significa cada parte

virtual → permite que o método seja sobrescrito (override) em classes derivadas.

= 0 → indica que é puro, ou seja, a classe não fornece implementação (é abstrata).

override (na implementação) → avisa ao compilador que você está substituindo um método virtual da base.


benefícios do uso de virtual

✅ Abstração — você programa contra a interface (IClientRepository* repo) e não a implementação.
✅ Inversão de dependência — facilita testes unitários (mockar um repositório sem precisar do banco).
✅ Flexibilidade — no futuro, pode ter ClientRepositoryPostgres, ClientRepositoryInMemory, etc.
✅ Polimorfismo — permite trocar a implementação sem mudar quem usa a interface.