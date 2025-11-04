#ifndef ADDRESS_H
#define ADDRESS_H

#include <string>
#include <chrono>

class Address {
private:
    long long id_;         
    long long client_id_;   // FK do cliente
    std::string street_;
    std::string number_;
    std::string city_;
    std::string state_;
    std::string zip_;
    std::string addressType_;
    std::chrono::system_clock::time_point createDate_;

public:
    // Construtor padrão (necessário para ORM / deserialização)
    Address();

    // Construtor de domínio
    Address(const std::string& street,
            const std::string& number,
            const std::string& city,
            const std::string& state,
            const std::string& zip,
            const std::string& addressType);

    // Getters
    long long getId() const { return id_; }
    long long getClientId() const { return client_id_; }
    const std::string& getStreet() const { return street_; }
    const std::string& getNumber() const { return number_; }
    const std::string& getCity() const { return city_; }
    const std::string& getState() const { return state_; }
    const std::string& getZip() const { return zip_; }
    const std::string& getAddressType() const { return addressType_; }
    const std::chrono::system_clock::time_point& getCreateDate() const { return createDate_; }

    // Setters
    void setId(long long id) { id_ = id; }
    void setClientId(long long client_id) { client_id_ = client_id; }
    void setStreet(const std::string& street) { street_ = street; }
    void setNumber(const std::string& number) { number_ = number; }
    void setCity(const std::string& city) { city_ = city; }
    void setState(const std::string& state) { state_ = state; }
    void setZip(const std::string& zip) { zip_ = zip; }
    void setAddressType(const std::string& type) { addressType_ = type; }
    void setCreateDate(const std::chrono::system_clock::time_point& date) { createDate_ = date; }
};

#endif // ADDRESS_H
