cmake_minimum_required(VERSION 3.20...4.1)
cmake_policy(VERSION 3.10)

project(e_cocin CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_WARN_DEPRECATED OFF)

# --- Deps via FetchContent (json, spdlog, Catch2, oatpp) ---
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(cmake/third_party.cmake)

# --- SQLite ---
find_package(SQLite3 REQUIRED)

# --- Executável principal ---
add_executable(e_cocin
  src/ECocinApplication.cpp
)

# Fonte(s) do projeto
target_sources(e_cocin PRIVATE
  src/domain/entities/Client.cpp
  src/infra/db/SqliteConnection.cpp
  src/infra/repositories/sqlite/ClientRepositorySqlite.cpp
  src/services/ClientService.cpp
  src/domain/entities/Product.cpp
  src/services/ProductService.cpp
  src/infra/repositories/sqlite/ProductRepositorySqlite.cpp
  src/domain/entities/Address.cpp
  src/services/AddressService.cpp
  src/infra/repositories/sqlite/AddressRepositorySqlite.cpp
  src/domain/entities/Order.cpp
  src/services/OrderService.cpp
  src/infra/repositories/sqlite/OrderRepositorySqlite.cpp

)


set_target_properties(e_cocin PROPERTIES OUTPUT_NAME "e-cocin")

# includes locais do projeto
target_include_directories(e_cocin PRIVATE src)

# (Opcional/útil) Força includes do oatpp para o compilador/IntelliSense
# Faça isso *depois* do add_executable
if(DEFINED oatpp_SOURCE_DIR)
  target_include_directories(e_cocin PRIVATE
    ${oatpp_SOURCE_DIR}/src
    ${oatpp_BINARY_DIR}/src
  )
endif()

# --- Mapeamento robusto do SQLite ---
set(_sqlite_inc "")
set(_sqlite_lib "")

if (TARGET SQLite::SQLite3)
  get_target_property(_sqlite_inc SQLite::SQLite3 INTERFACE_INCLUDE_DIRECTORIES)
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

if (_sqlite_inc)
  target_include_directories(e_cocin PRIVATE ${_sqlite_inc})
endif()

if (TARGET SQLite::SQLite3)
  target_link_libraries(e_cocin PRIVATE SQLite::SQLite3)
elseif (_sqlite_lib)
  target_link_libraries(e_cocin PRIVATE ${_sqlite_lib})
else()
  message(FATAL_ERROR "SQLite3 não encontrado. Instale: pacman -S --needed mingw-w64-x86_64-sqlite3")
endif()

# --- Outras libs ---
target_link_libraries(e_cocin
  PRIVATE
    nlohmann_json::nlohmann_json
    spdlog::spdlog_header_only
)

# oatpp pode vir como 'oatpp::oatpp' (exports) ou 'oatpp' (add_subdirectory)
if (TARGET oatpp::oatpp)
  target_link_libraries(e_cocin PRIVATE oatpp::oatpp)
elseif (TARGET oatpp)
  add_library(oatpp::oatpp ALIAS oatpp)
  target_link_libraries(e_cocin PRIVATE oatpp::oatpp)
else()
  message(FATAL_ERROR "Nenhum alvo do oatpp foi criado. Verifique cmake/third_party.cmake.")
endif()

# --- Testes (opcional) ---
enable_testing()
add_executable(unit_tests tests/test_example.cpp)
target_link_libraries(unit_tests PRIVATE Catch2::Catch2WithMain)
add_test(NAME example_test COMMAND unit_tests)

# --- Logs úteis ---
message(STATUS "oatpp_SOURCE_DIR = ${oatpp_SOURCE_DIR}")
message(STATUS "oatpp_BINARY_DIR = ${oatpp_BINARY_DIR}")
message(STATUS "SQLite include dirs: ${_sqlite_inc}")
message(STATUS "SQLite lib: ${_sqlite_lib}")
