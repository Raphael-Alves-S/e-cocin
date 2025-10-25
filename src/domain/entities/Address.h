#ifndef ADDRESS_H
#define ADDRESS_H

#include <string>
#include "core/Uuid.h"

class Address {
private:
    long long id_;          // Identificador único do endereço
    long long client_id_;   // Identificador do cliente ao qual o endereço pertence
    std::string street_;    // rua
    std::string number_;    // número
    std::string city_;      // cidade
    std::string state_;     // estado
    std::string zip_;       // CEP
    std::string addressType_;   // tipo de endereço (residencial, comercial, etc.)

public:
    // Construtores
    Address();
    Address(const std::string& street,  // rua
            const std::string& number,  // número
            const std::string& city,    // cidade
            const std::string& state,   // estado
            const std::string& zip,     // CEP
            const std::string& addressType);    // tipo de endereço (residencial, comercial, etc.)
    // Getters
    long long           getId() const { return id_; }
    long long           getClientId() const { return client_id_; }
    const std::string& getStreet() const { return street_; }
    const std::string& getNumber() const { return number_; }
    const std::string& getCity() const { return city_; }
    const std::string& getState() const { return state_; }
    const std::string& getZip() const { return zip_; }
    const std::string& getAddressType() const { return addressType_; }

    // Setters
    void setId(long long id) { id_ = id; }
    void setClientId(long long client_id) { client_id_ = client_id; }
    void setStreet(const std::string& street) { street_ = street; }
    void setNumber(const std::string& number) { number_ = number; }
    void setCity(const std::string& city) { city_ = city; }
    void setState(const std::string& state) { state_ = state; }
    void setZip(const std::string& zip) { zip_ = zip; }
    void setAddressType(const std::string& addressType) { addressType_ = addressType; }

};
#endif