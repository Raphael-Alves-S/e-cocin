#ifndef ADDRESS_H
#define ADDRESS_H

#include <string>
#include "core/Uuid.h"

class Address {
private:
    long long id_;
    long long client_id_;
    std::string street_;
    std::string number_;
    std::string city_;
    std::string state_;
    std::string zip_;
    std::string addressType_;

public:
    // Construtores
    Address();
    Address(const std::string& street,
            const std::string& number,
            const std::string& city,
            const std::string& state,
            const std::string& zip,
            const std::string& addressType);