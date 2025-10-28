#include "AddressRepositorySqlite.h"
#include "Helpers.h"
#include <chrono>

using namespace ecocin;
using infra::repositories::sqlite::AddressRepositorySqlite;

static Address row_to_address(sqlite3_stmt* s) {
    Address address;
    address.setId(static_cast<long long>(sqlite3_column_int64(s, 0)));
    address.setClientId(static_cast<long long>(sqlite3_column_int64(s, 1)));
    address.setStreet(reinterpret_cast<const char*>(sqlite3_column_text(s, 2)));
    address.setNumber(reinterpret_cast<const char*>(sqlite3_column_text(s, 3)));
    address.setCity(reinterpret_cast<const char*>(sqlite3_column_text(s, 4)));
    address.setState(reinterpret_cast<const char*>(sqlite3_column_text(s, 5)));
    address.setZip(reinterpret_cast<const char*>(sqlite3_column_text(s, 6)));
    address.setAddressType(reinterpret_cast<const char*>(sqlite3_column_text(s, 7)));

    long long secs = sqlite3_column_int64(s, 8);
    address.setCreateDate(std::chrono::system_clock::time_point{std::chrono::seconds{secs}});
    return address;
}

Address AddressRepositorySqlite::create(const Address& in) {
    Address address = in;
    const char* sql = "INSERT INTO addresses(client_id,street,number,city,state,zip,address_type,create_date) VALUES(?,?,?,?,?,?,?,?)";
    sqlite3_stmt* st = nullptr;
    auto now = std::chrono::system_clock::now();
    address.setCreateDate(now);
    sqlite_check(sqlite3_prepare_v2(connection_.raw(), sql, -1, &st, nullptr), connection_.raw(), "prepare insert address");
    sqlite3_bind_int64(st, 1, address.getClientId());
    sqlite3_bind_text(st, 2, address.getStreet().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 3, address.getNumber().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 4, address.getCity().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 5, address.getState().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 6, address.getZip().c_str(), -1 , SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 7, address.getAddressType().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(st, 8, std::chrono::duration_cast<std::chrono::seconds>(address.getCreateDate().time_since_epoch()).count());
    sqlite_check(sqlite3_step(st), connection_.raw(), "step insert address");
    sqlite3_finalize(st);
    address.setId(static_cast<long long>(sqlite3_last_insert_rowid(connection_.raw())));
    return address;
}

std::optional<Address> AddressRepositorySqlite::findById(long long id) {
    const char* sql = "SELECT id,client_id,street,number,city,state,zip,address_type,create_date FROM addresses WHERE id=?";
    sqlite3_stmt* st = nullptr;
    sqlite_check(sqlite3_prepare_v2(connection_.raw(), sql, -1, &st, nullptr), connection_.raw(), "prepare get address");
    sqlite3_bind_int64(st, 1, id);
    if (sqlite3_step(st) == SQLITE_ROW) {
        auto addr = row_to_address(st);
        sqlite3_finalize(st);
        return addr;
    }
    sqlite3_finalize(st);
    return std::nullopt;
}
std::vector<Address> AddressRepositorySqlite::listAll() {
    const char* sql = "SELECT id,client_id,street,number,city,state,zip,address_type,create_date FROM addresses ORDER BY id DESC";
    sqlite3_stmt* st = nullptr;
    sqlite_check(sqlite3_prepare_v2(connection_.raw(), sql, -1, &st, nullptr), connection_.raw(), "prepare list addresses");
    std::vector<Address> out;
    while (sqlite3_step(st) == SQLITE_ROW) out.push_back(row_to_address(st));
    sqlite3_finalize(st);
    return out;
}   

bool AddressRepositorySqlite::update(const Address& addr) {
    const char* sql = "UPDATE addresses SET client_id=?, street=?, number=?, city=?, state=?, zip=?, address_type=? WHERE id=?";
    sqlite3_stmt* st = nullptr;
    sqlite_check(sqlite3_prepare_v2(connection_.raw(), sql, -1, &st, nullptr), connection_.raw(), "prepare update address");
    sqlite3_bind_int64(st, 1, addr.getClientId());
    sqlite3_bind_text(st, 2, addr.getStreet().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 3, addr.getNumber().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 4, addr.getCity().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 5, addr.getState().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 6, addr.getZip().c_str(), -1 , SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 7, addr.getAddressType().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(st, 8, addr.getId());
    sqlite_check(sqlite3_step(st), connection_.raw(), "step update address");
    int changes = sqlite3_changes(connection_.raw());
    sqlite3_finalize(st);
    return changes > 0;
}   

bool AddressRepositorySqlite::remove(long long id) {
    const char* sql = "DELETE FROM addresses WHERE id=?";
    sqlite3_stmt* st = nullptr;
    sqlite_check(sqlite3_prepare_v2(connection_.raw(), sql, -1, &st, nullptr), connection_.raw(), "prepare delete address");
    sqlite3_bind_int64(st, 1, id);
    sqlite_check(sqlite3_step(st), connection_.raw(), "step delete address");
    int changes = sqlite3_changes(connection_.raw());
    sqlite3_finalize(st);
    return changes > 0;
}   