include(FetchContent)

# nlohmann/json (header-only)
FetchContent_Declare(nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)

# spdlog (header-only)
FetchContent_Declare(spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.13.0
)
FetchContent_MakeAvailable(spdlog)

# Catch2 (tests)
FetchContent_Declare(catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.2
)
FetchContent_MakeAvailable(catch2)

# --- oatpp (REST framework) ---
include(FetchContent)

# Evitar compilar testes/exemplos do oatpp
set(OATPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(OATPP_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(oatpp
  GIT_REPOSITORY https://github.com/oatpp/oatpp.git
  GIT_TAG        master
)

# Caminho 1: CMake 3.14+ com MakeAvailable
# Tenta usar MakeAvailable, mas alguns ambientes não registram o alvo; então garantimos com GetProperties/Populate.
FetchContent_MakeAvailable(oatpp)

# Caminho 2 (robusto): se ainda não estiver populado, populamos e chamamos add_subdirectory manualmente
FetchContent_GetProperties(oatpp)
if(NOT oatpp_POPULATED)
  FetchContent_Populate(oatpp)
  add_subdirectory(${oatpp_SOURCE_DIR} ${oatpp_BINARY_DIR})
endif()
# --- Fim oatpp ---

