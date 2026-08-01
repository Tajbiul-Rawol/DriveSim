// =============================================================================
//  DriveSim  -  DataStore.cpp
// =============================================================================
#include "DataStore.h"

#include <algorithm>
#include <cctype>

namespace drivesim {

namespace {
// Extract the trailing integer of an id such as "VH-1042" -> 1042. Returns 0
// when no trailing digits are present.
int trailingNumber(const std::string& id) {
    int i = static_cast<int>(id.size()) - 1;
    while (i >= 0 && std::isdigit(static_cast<unsigned char>(id[i]))) --i;
    std::string digits = id.substr(i + 1);
    if (digits.empty()) return 0;
    try { return std::stoi(digits); } catch (...) { return 0; }
}
} // namespace

// ---- Vehicles -------------------------------------------------------------
void DataStore::addVehicle(std::unique_ptr<Vehicle> v) {
    if (v) vehicles_.push_back(std::move(v));
}

Vehicle* DataStore::findVehicle(const std::string& id) {
    for (auto& v : vehicles_)
        if (v->id() == id) return v.get();
    return nullptr;
}

bool DataStore::removeVehicle(const std::string& id) {
    for (auto it = vehicles_.begin(); it != vehicles_.end(); ++it) {
        if ((*it)->id() == id) { vehicles_.erase(it); return true; }
    }
    return false;
}

std::vector<Vehicle*> DataStore::allVehicles() {
    std::vector<Vehicle*> out;
    for (auto& v : vehicles_) out.push_back(v.get());
    return out;
}

std::vector<Vehicle*> DataStore::availableVehicles() {
    std::vector<Vehicle*> out;
    for (auto& v : vehicles_)
        if (v->status() == VehicleStatus::Available) out.push_back(v.get());
    return out;
}

std::vector<Vehicle*> DataStore::vehiclesByCategory(const std::string& category) {
    std::string c = toLower(trim(category));
    std::vector<Vehicle*> out;
    for (auto& v : vehicles_)
        if (toLower(v->category()) == c) out.push_back(v.get());
    return out;
}

std::vector<Vehicle*> DataStore::vehiclesUnderMaxRate(double maxEffectiveRate) {
    std::vector<Vehicle*> out;
    for (auto& v : vehicles_)
        if (v->effectiveDailyRate() <= maxEffectiveRate) out.push_back(v.get());
    return out;
}

// ---- Users ----------------------------------------------------------------
void DataStore::addUser(std::unique_ptr<User> u) {
    if (u) users_.push_back(std::move(u));
}

User* DataStore::findUserByUsername(const std::string& username) {
    std::string u = toLower(trim(username));
    for (auto& user : users_)
        if (toLower(user->username()) == u) return user.get();
    return nullptr;
}

User* DataStore::findUserById(const std::string& id) {
    for (auto& user : users_)
        if (user->id() == id) return user.get();
    return nullptr;
}

std::vector<Customer*> DataStore::customers() {
    std::vector<Customer*> out;
    for (auto& user : users_)
        if (user->role() == UserRole::Customer)
            out.push_back(static_cast<Customer*>(user.get()));
    return out;
}

// ---- Bookings -------------------------------------------------------------
void DataStore::addBooking(const Booking& b) { bookings_.push_back(b); }

Booking* DataStore::findBooking(const std::string& id) {
    for (auto& b : bookings_)
        if (b.id() == id) return &b;
    return nullptr;
}

std::vector<Booking*> DataStore::bookingsForCustomer(const std::string& customerId) {
    std::vector<Booking*> out;
    for (auto& b : bookings_)
        if (b.customerId() == customerId) out.push_back(&b);
    return out;
}

std::vector<Booking*> DataStore::bookingsForVehicle(const std::string& vehicleId) {
    std::vector<Booking*> out;
    for (auto& b : bookings_)
        if (b.vehicleId() == vehicleId) out.push_back(&b);
    return out;
}

// ---- ID generation --------------------------------------------------------
// IDs are prefixed and numbered from a floor of 1001. The next id is one more
// than the highest existing suffix, so ids stay unique after loading a store.
std::string DataStore::nextVehicleId() {
    int mx = 1000;
    for (auto& v : vehicles_) mx = std::max(mx, trailingNumber(v->id()));
    return "VH-" + std::to_string(mx + 1);
}

std::string DataStore::nextCustomerId() {
    int mx = 1000;
    for (auto& u : users_)
        if (u->role() == UserRole::Customer) mx = std::max(mx, trailingNumber(u->id()));
    return "CU-" + std::to_string(mx + 1);
}

std::string DataStore::nextBookingId() {
    int mx = 1000;
    for (auto& b : bookings_) mx = std::max(mx, trailingNumber(b.id()));
    return "BK-" + std::to_string(mx + 1);
}

} // namespace drivesim
