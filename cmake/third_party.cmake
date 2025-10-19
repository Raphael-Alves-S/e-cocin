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

# oat++ (web)
FetchContent_Declare(oatpp
  GIT_REPOSITORY https://github.com/oatpp/oatpp.git
  GIT_TAG 1.3.0
)
FetchContent_MakeAvailable(oatpp)
