// =============================================================================
//  DriveSim  -  User.h
//  Role-based user model. A User carries credentials; Customer and Admin
//  specialise it. role() drives access control at the menu layer.
// =============================================================================
#ifndef DRIVESIM_USER_H
#define DRIVESIM_USER_H

#include <memory>
#include <string>

#include "Common.h"

namespace drivesim {

class User {
public:
    // passwordHash is expected to already be hashed (see simpleHash). Callers
    // registering a new user should hash the plaintext first, or use
    // setPassword() which hashes for them.
    User(std::string id, std::string username, std::string passwordHash,
         std::string fullName);
    virtual ~User() = default;

    virtual UserRole role() const = 0;

    bool checkPassword(const std::string& plaintext) const;
    void setPassword(const std::string& plaintext);

    const std::string& id()           const { return id_; }
    const std::string& username()     const { return username_; }
    const std::string& fullName()     const { return fullName_; }
    const std::string& passwordHash() const { return passwordHash_; }

    virtual std::string email() const { return ""; }
    virtual std::string phone() const { return ""; }

    virtual std::string toCsv() const = 0;
    static std::unique_ptr<User> fromCsv(const std::string& line);

protected:
    std::string id_;
    std::string username_;
    std::string passwordHash_;
    std::string fullName_;
};

class Customer : public User {
public:
    Customer(std::string id, std::string username, std::string passwordHash,
             std::string fullName, std::string email, std::string phone);

    UserRole    role()  const override { return UserRole::Customer; }
    std::string email() const override { return email_; }
    std::string phone() const override { return phone_; }
    std::string toCsv() const override;

private:
    std::string email_;
    std::string phone_;
};

class Admin : public User {
public:
    using User::User;
    UserRole    role()  const override { return UserRole::Admin; }
    std::string toCsv() const override;
};

} // namespace drivesim

#endif // DRIVESIM_USER_H
