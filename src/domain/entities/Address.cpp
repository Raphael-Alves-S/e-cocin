#include "Address.h"
#include <chrono>

// Construtor padrão
Address::Address()
    : id_(0),
      client_id_(0),
      street_(""),
      number_(""),
      city_(""),
      state_(""),
      zip_(""),
      addressType_(""),
      createDate_(std::chrono::system_clock::now()) {}

// Construtor de domínio
Address::Address(const std::string& street,
                 const std::string& number,
                 const std::string& city,
                 const std::string& state,
                 const std::string& zip,
                 const std::string& addressType)
    : id_(0),
      client_id_(0),
      street_(street),
      number_(number),
      city_(city),
      state_(state),
      zip_(zip),
      addressType_(addressType),
      createDate_(std::chrono::system_clock::now()) {}
