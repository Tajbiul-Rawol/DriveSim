// =============================================================================
//  DriveSim  -  User.cpp
// =============================================================================
#include "User.h"

namespace drivesim {

User::User(std::string id, std::string username, std::string passwordHash,
           std::string fullName)
    : id_(std::move(id)), username_(std::move(username)),
      passwordHash_(std::move(passwordHash)), fullName_(std::move(fullName)) {}

bool User::checkPassword(const std::string& plaintext) const {
    return simpleHash(plaintext) == passwordHash_;
}

void User::setPassword(const std::string& plaintext) {
    passwordHash_ = simpleHash(plaintext);
}

Customer::Customer(std::string id, std::string username, std::string passwordHash,
                   std::string fullName, std::string email, std::string phone)
    : User(std::move(id), std::move(username), std::move(passwordHash),
           std::move(fullName)),
      email_(std::move(email)), phone_(std::move(phone)) {}

// Column order: role, id, username, passwordHash, fullName, email, phone
std::string Customer::toCsv() const {
    return makeCsvLine({ toString(role()), id_, username_, passwordHash_,
                         fullName_, email_, phone_ });
}

std::string Admin::toCsv() const {
    return makeCsvLine({ toString(role()), id_, username_, passwordHash_,
                         fullName_, "", "" });
}

std::unique_ptr<User> User::fromCsv(const std::string& line) {
    auto f = parseCsvLine(line);
    if (f.size() < 5) return nullptr;
    UserRole role = UserRole::Customer;
    userRoleFromString(f[0], role);
    const std::string& id       = f[1];
    const std::string& username = f[2];
    const std::string& hash     = f[3];
    const std::string& name     = f[4];
    std::string email = f.size() > 5 ? f[5] : "";
    std::string phone = f.size() > 6 ? f[6] : "";

    if (role == UserRole::Admin)
        return std::make_unique<Admin>(id, username, hash, name);
    return std::make_unique<Customer>(id, username, hash, name, email, phone);
}

} // namespace drivesim
