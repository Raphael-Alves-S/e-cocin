#include "Client.h"
#include <algorithm>
#include <cctype>
#include <chrono>

// Helpers internos 
static inline std::string trim_copy(std::string s){
    auto not_space = [](unsigned char ch){ return !std::isspace(ch); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), not_space));
    s.erase(std::find_if(s.rbegin(), s.rend(), not_space).base(), s.end());
    return s;
}
// Construtores
Client::Client()
    : createDate_(std::chrono::system_clock::now()) {}
Client::Client(const std::string& name,
               const std::string& email,
               const std::string& cpf)
    : name_(trim_copy(name)),
      email_(trim_copy(email)),
      cpf_(trim_copy(cpf)),
      createDate_(std::chrono::system_clock::now()) {}

// utilitário estático
std::string Client::trim(const std::string& s){
    return trim_copy(s);
}     