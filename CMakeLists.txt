cmake_minimum_required(VERSION 3.20...4.1)
cmake_policy(VERSION 3.10)

# ---------- Projeto ----------
project(e_cocin CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_WARN_DEPRECATED OFF)

# ---------- Dependências (FetchContent) ----------
# Mantemos json, spdlog e Catch2 via third_party.cmake
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(cmake/third_party.cmake)

# ---------- SQLite (MSYS2 / MinGW64) ----------
# Requer o pacote: pacman -S --needed mingw-w64-x86_64-sqlite3
find_package(SQLite3 REQUIRED)

# ---------- Executável principal ----------
add_executable(e_cocin
  src/ECocinApplication.cpp
)
# nome do binário final
set_target_properties(e_cocin PROPERTIES OUTPUT_NAME "e-cocin")

# includes locais do projeto
target_include_directories(e_cocin PRIVATE src)

# ---------- Mapeamento robusto do SQLite (build + IntelliSense) ----------
# 1) Tenta obter includes/lib direto do imported target SQLite::SQLite3
# 2) Se não vier, usa variáveis do FindSQLite3 (SQLite3_INCLUDE_DIRS/LIBRARIES)
# 3) Fallback MSYS2: caminhos padrão do mingw64
set(_sqlite_inc "")
set(_sqlite_lib "")

if (TARGET SQLite::SQLite3)
  get_target_property(_sqlite_inc SQLite::SQLite3 INTERFACE_INCLUDE_DIRECTORIES)
  # IMPORTED_LOCATION pode não estar setado em todas as plataformas; foco no include para IntelliSense
  get_target_property(_sqlite_lib SQLite::SQLite3 IMPORTED_LOCATION)
endif()

if (NOT _sqlite_inc AND DEFINED SQLite3_INCLUDE_DIRS)
  set(_sqlite_inc ${SQLite3_INCLUDE_DIRS})
endif()

if (NOT _sqlite_lib AND DEFINED SQLite3_LIBRARIES)
  set(_sqlite_lib ${SQLite3_LIBRARIES})
endif()

# Fallback MSYS2 típico
if (NOT _sqlite_inc AND EXISTS "C:/msys64/mingw64/include/sqlite3.h")
  list(APPEND _sqlite_inc "C:/msys64/mingw64/include")
endif()
if (NOT _sqlite_lib AND EXISTS "C:/msys64/mingw64/lib/libsqlite3.dll.a")
  list(APPEND _sqlite_lib "C:/msys64/mingw64/lib/libsqlite3.dll.a")
endif()

# Aplica includes do SQLite explicitamente ao alvo (ajuda muito o IntelliSense)
if (_sqlite_inc)
  target_include_directories(e_cocin PRIVATE ${_sqlite_inc})
endif()

# Linka SQLite (preferindo o imported target moderno)
if (TARGET SQLite::SQLite3)
  target_link_libraries(e_cocin PRIVATE SQLite::SQLite3)
elseif (_sqlite_lib)
  target_link_libraries(e_cocin PRIVATE ${_sqlite_lib})
else()
  message(FATAL_ERROR "SQLite3 não encontrado. Instale: pacman -S --needed mingw-w64-x86_64-sqlite3")
endif()

# ---------- Outras libs ----------
target_link_libraries(e_cocin
  PRIVATE
    nlohmann_json::nlohmann_json
    spdlog::spdlog_header_only
)

# ---------- Testes (opcional) ----------
enable_testing()
add_executable(unit_tests tests/test_example.cpp)
target_link_libraries(unit_tests PRIVATE Catch2::Catch2WithMain)
add_test(NAME example_test COMMAND unit_tests)

# ---------- Logs úteis ----------
message(STATUS "SQLite include dirs: ${_sqlite_inc}")
message(STATUS "SQLite lib: ${_sqlite_lib}")
