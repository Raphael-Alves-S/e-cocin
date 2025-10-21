#ifndef ECOCIN_CORE_UUID_H
#define ECOCIN_CORE_UUID_H

#include <string>
#include <random>
#include <sstream>
#include <iomanip>

// Classe utilitária para geração e manipulação de UUID v4
// Mantida inline, pois é leve e não requer .cpp adicional.

namespace ecocin::core {

class Uuid {
private:
    std::string value_;

public:
    // Construtores
    Uuid() = default;
    explicit Uuid(const std::string& v) : value_(v) {}

    // Retorna o valor do UUID em formato string
    const std::string& str() const { return value_; }

    // Indica se o UUID é vazio
    bool empty() const { return value_.empty(); }

    // Comparações
    bool operator==(const Uuid& other) const { return value_ == other.value_; }
    bool operator!=(const Uuid& other) const { return !(*this == other); }

    // Cria um UUID v4 aleatório (padrão RFC 4122)
    static Uuid v4() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(0, 15);
        std::uniform_int_distribution<int> dist2(8, 11);

        std::stringstream ss;
        ss << std::hex;
        for (int i = 0; i < 8; i++) ss << dist(gen);
        ss << "-";
        for (int i = 0; i < 4; i++) ss << dist(gen);
        ss << "-4"; // versão
        for (int i = 0; i < 3; i++) ss << dist(gen);
        ss << "-";
        ss << dist2(gen);
        for (int i = 0; i < 3; i++) ss << dist(gen);
        ss << "-";
        for (int i = 0; i < 12; i++) ss << dist(gen);

        return Uuid(ss.str());
    }
};

// Função auxiliar global (atalho)
inline Uuid uuid_v4() { return Uuid::v4(); }

} // namespace ecocin::core

#endif // ECOCIN_CORE_UUID_H
