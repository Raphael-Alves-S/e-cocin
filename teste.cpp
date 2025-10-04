#include <iostream>
#include <string>
#include "json.hpp"
// Supondo que você use uma biblioteca como cpp-httplib para a requisição
// #include <httplib.h> 

// Definição da struct WeatherDTO
struct WeatherDTO {
    std::string city;
    double temperature;
    std::string condition;
};

// Função para deserialização automática do JSON para WeatherDTO
void from_json(const nlohmann::json& j, WeatherDTO& w) {
    j.at("city").get_to(w.city);
    j.at("temperature").get_to(w.temperature);
    j.at("condition").get_to(w.condition);
}

int main() {
    // Simulação da string de resposta da API
    std::string json_response_string = R"({"city": "Recife", "temperature": 28.5, "condition": "Ensolarado"})";

    try {
        // Parse da string JSON para um objeto nlohmann::json
        nlohmann::json j = nlohmann::json::parse(json_response_string);

        // Conversão do objeto json para a struct WeatherDTO
        WeatherDTO weather_data = j.get<WeatherDTO>();

        // Agora você pode usar os dados de forma segura e legível
        std::cout << "Cidade: " << weather_data.city << std::endl;
        std::cout << "Temperatura: " << weather_data.temperature << "°C" << std::endl;
        std::cout << "Condição: " << weather_data.condition << std::endl;

    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "Erro ao analisar o JSON: " << e.what() << std::endl;
    }

    return 0;
}