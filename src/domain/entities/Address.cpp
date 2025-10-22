#include "Address.h"

Address::Address(const std::string& street,
                const std::string& number,
                const std::string& city,
                const std::string& state,
                const std::string& zip,
                const std::string& addressType)
    : street_(street),
      number_(number),
      city_(city),
      state_(state),
      zip_(zip),
      addressType_(addressType) {}

// Address::Address()
//     : id_(0),
//       client_id_(0),
//       street_(""),
//       number_(""),
//       city_(""),
//       state_(""),
//       zip_(""),
//       addressType_("") {}