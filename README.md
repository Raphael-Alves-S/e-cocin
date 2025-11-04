
# e-cocin — Setup (Windows + MSYS2/MinGW64) e Build

Este guia explica como configurar o ambiente de desenvolvimento, construir e executar o projeto e-cocin.

## Visão Geral

e-cocin é uma API REST para um sistema de e-commerce, construída em C++ com o framework Oat++.

## Funcionalidades

*   Gerenciamento de Clientes
*   Gerenciamento de Produtos
*   Gerenciamento de Endereços
*   Criação e consulta de Pedidos

## Tecnologias Utilizadas

*   **C++20**
*   **Oat++**: Framework para desenvolvimento de APIs REST.
*   **CMake**: Sistema de build.
*   **SQLite**: Banco de dados.
*   **MinGW64 (MSYS2)**: Toolchain para compilação no Windows.

---

## 1. Configuração do Ambiente (Windows + MSYS2/MinGW64)

Este guia explica **exatamente** como reproduzir o ambiente que usamos:

- **MinGW64 (MSYS2)** como toolchain (GCC 15+)
- **CMake 4.x**
- Dependências via **FetchContent** (nlohmann/json, spdlog, Catch2)
- **SQLite** do MSYS2 (via `find_package(SQLite3)`)

> Estrutura atual do projeto:
>
> ```
> e-cocin/
> ├─ .vscode/
> ├─ build/
> ├─ cmake/
> │  └─ third_party.cmake
> ├─ src/
> │  ├─ app/
> │  ├─ controllers/
> │  ├─ domain/
> │  ├─ infra/
> │  ├─ services/
> │  └─ ECocinApplication.cpp
> ├─ tests/
> │  └─ test_example.cpp
> ├─ .gitignore
> ├─ CMakeLists.txt
> ├─ Documentação_POO.md
> └─ README.md
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

# Instalar toolchain, utilitários e SQLite
pacman -S --needed base-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake git mingw-w64-x86_64-sqlite3

# (opcional) Ninja para builds rápidos
pacman -S --needed mingw-w64-x86_64-ninja
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

## 3) CMakeLists.txt

O `CMakeLists.txt` principal orquestra o build. Ele é responsável por:
*   Definir a versão do C++ (C++20).
*   Incluir o `third_party.cmake` para gerenciar dependências externas.
*   Encontrar o pacote do SQLite3.
*   Definir o executável principal (`e-cocin`) e seus arquivos-fonte.
*   Vincular todas as bibliotecas necessárias (Oat++, SQLite, etc.).
*   Configurar o alvo de testes.

---

## 4) Gerenciamento de Dependências (`cmake/third_party.cmake`)

As dependências externas são gerenciadas pelo `FetchContent` do CMake no arquivo `cmake/third_party.cmake`. Isso garante que as versões corretas das bibliotecas sejam baixadas e compiladas junto com o projeto.

As dependências incluem:
*   **nlohmann/json**: Para manipulação de JSON.
*   **spdlog**: Para logging.
*   **Catch2**: Para os testes unitários.
*   **oatpp**: O framework web principal.

---

## 5) Estrutura do Código-Fonte (`src/`)

O código-fonte é organizado em uma arquitetura de camadas para promover a separação de responsabilidades:

*   `app/`: Contém a lógica de inicialização da aplicação, como as migrações do banco de dados.
*   `controllers/`: Responsável por receber as requisições HTTP, validar os dados e interagir com a camada de serviço.
*   `domain/`: Contém as entidades de negócio (`Client`, `Product`, etc.) e as interfaces dos repositórios.
*   `infra/`: Implementação da infraestrutura, como a conexão com o banco de dados e as implementações concretas dos repositórios.
*   `services/`: Contém a lógica de negócio da aplicação.
*   `ECocinApplication.cpp`: O ponto de entrada da aplicação (`main`).


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

Após a execução, a API estará disponível em `http://localhost:8000`.

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
🚀 API rodando em http://localhost:8000
1/1 Test #1: example_test ..........   Passed
```

Tudo certo 🎉

---

## 11) Rotas da API

A seguir, a lista de rotas disponíveis na API.

### Clientes (`/clients`)

*   `POST /clients`: Cria um novo cliente.
    *   **Body**: `{ "name": "string", "email": "string", "cpf": "string" }`
*   `GET /clients`: Lista todos os clientes.
*   `GET /clients/{id}`: Busca um cliente pelo ID.
*   `GET /clients/cpf/{cpf}`: Busca um cliente pelo CPF.

### Produtos (`/products`)

*   `POST /products`: Cria um novo produto.
    *   **Body**: `{ "name": "string", "description": "string", "price": number, "stockQuantity": integer, "isActive": boolean, "sku": "string" }`
*   `GET /products`: Lista todos os produtos.
*   `GET /products/{id}`: Busca um produto pelo ID.
*   `GET /products/sku/{sku}`: Busca um produto pelo SKU.
*   `PUT /products/{id}`: Atualiza um produto.
    *   **Body**: `{ "name": "string", "description": "string", "price": number, "stockQuantity": integer, "isActive": boolean, "sku": "string" }`
*   `DELETE /products/{id}`: Remove um produto.

### Endereços (`/addresses`)

*   `POST /addresses`: Cria um novo endereço para um cliente.
    *   **Body**: `{ "cpf": "string", "street": "string", "number": "string", "city": "string", "state": "string", "zip": "string", "addressType": "string" }`
*   `GET /addresses`: Lista todos os endereços.
*   `GET /clients/{cpf}/addresses`: Lista todos os endereços de um cliente específico.

### Pedidos (`/orders`)

*   `POST /orders`: Cria um novo pedido.
    *   **Body**: `{ "cpf": "string", "sku": "string", "shippingAddressType": "string", "quantity": integer }`
*   `GET /orders?cpf={cpf}`: Lista todos os pedidos de um cliente.
