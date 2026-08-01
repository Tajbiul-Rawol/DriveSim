// =============================================================================
//  DriveSim  -  Persistence.cpp
// =============================================================================
#include "Persistence.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

namespace drivesim {

Persistence::Persistence(std::string dataDir) : dataDir_(std::move(dataDir)) {
    vehiclesPath_ = dataDir_ + "/vehicles.csv";
    usersPath_    = dataDir_ + "/users.csv";
    bookingsPath_ = dataDir_ + "/bookings.csv";
}

void Persistence::ensureDir() const {
    std::error_code ec;
    if (!fs::exists(dataDir_, ec)) fs::create_directories(dataDir_, ec);
}

void Persistence::load(DataStore& store) {
    ensureDir();

    const bool freshInstall = !fs::exists(usersPath_) || !fs::exists(vehiclesPath_);
    if (freshInstall) {
        seed(store);
        save(store);
        std::cout << "[setup] No data found - seeded a default admin account "
                     "and starter fleet.\n";
        return;
    }

    loadVehicles(store);
    loadUsers(store);
    loadBookings(store);
}

void Persistence::seed(DataStore& store) {
    // Default administrator (username: admin / password: admin123).
    store.addUser(std::make_unique<Admin>(
        "AD-1001", "admin", simpleHash("admin123"), "System Administrator"));

    // A demo customer (username: demo / password: demo123).
    store.addUser(std::make_unique<Customer>(
        "CU-1001", "demo", simpleHash("demo123"), "Demo Customer",
        "demo@example.com", "0400 000 000"));

    // Starter fleet spanning every category.
    struct Seed { const char* cat; const char* make; const char* model; int year; double rate; };
    const Seed fleet[] = {
        {"Sedan",     "Toyota",    "Camry",     2022,  55.0},
        {"Sedan",     "Mazda",     "3",         2021,  49.0},
        {"SUV",       "Toyota",    "RAV4",      2023,  70.0},
        {"SUV",       "Hyundai",   "Tucson",    2022,  65.0},
        {"Electric",  "Tesla",     "Model 3",   2023,  90.0},
        {"Electric",  "BYD",       "Atto 3",    2023,  75.0},
        {"LuxuryVan", "Mercedes",  "V-Class",   2022, 140.0},
        {"Sedan",     "Kia",       "Cerato",    2020,  45.0},
    };
    int seq = 1001;
    for (const auto& s : fleet) {
        std::string id = "VH-" + std::to_string(seq++);
        store.addVehicle(Vehicle::create(s.cat, id, s.make, s.model, s.year, s.rate));
    }
}

// ---- Loading --------------------------------------------------------------
namespace {
// Read non-empty, non-comment lines from a file.
std::vector<std::string> readDataLines(const std::string& path) {
    std::vector<std::string> lines;
    std::ifstream in(path);
    if (!in) return lines;
    std::string line;
    while (std::getline(in, line)) {
        std::string t = trim(line);
        if (t.empty() || t[0] == '#') continue;   // skip header/comment lines
        lines.push_back(line);
    }
    return lines;
}
} // namespace

void Persistence::loadVehicles(DataStore& store) {
    for (const auto& line : readDataLines(vehiclesPath_)) {
        auto v = Vehicle::fromCsv(line);
        if (v) store.addVehicle(std::move(v));
    }
}

void Persistence::loadUsers(DataStore& store) {
    for (const auto& line : readDataLines(usersPath_)) {
        auto u = User::fromCsv(line);
        if (u) store.addUser(std::move(u));
    }
}

void Persistence::loadBookings(DataStore& store) {
    for (const auto& line : readDataLines(bookingsPath_)) {
        Booking b;
        if (Booking::fromCsv(line, b)) store.addBooking(b);
    }
}

// ---- Saving ---------------------------------------------------------------
namespace {
// Write lines to a temp file then rename over the target - avoids leaving a
// half-written file if the process dies mid-write.
bool writeLines(const std::string& path, const std::string& header,
                const std::vector<std::string>& lines) {
    std::string tmp = path + ".tmp";
    {
        std::ofstream out(tmp, std::ios::trunc);
        if (!out) return false;
        out << header << "\n";
        for (const auto& l : lines) out << l << "\n";
    }
    std::error_code ec;
    fs::rename(tmp, path, ec);
    if (ec) {                                   // fall back to a direct copy
        std::ofstream out(path, std::ios::trunc);
        if (!out) return false;
        out << header << "\n";
        for (const auto& l : lines) out << l << "\n";
    }
    return true;
}
} // namespace

void Persistence::save(const DataStore& store) {
    ensureDir();
    saveVehicles(store);
    saveUsers(store);
    saveBookings(store);
}

void Persistence::saveVehicles(const DataStore& store) {
    std::vector<std::string> lines;
    for (const auto& v : store.vehicles()) lines.push_back(v->toCsv());
    writeLines(vehiclesPath_, "#id,category,make,model,year,baseDailyRate,status", lines);
}

void Persistence::saveUsers(const DataStore& store) {
    std::vector<std::string> lines;
    for (const auto& u : store.users()) lines.push_back(u->toCsv());
    writeLines(usersPath_, "#role,id,username,passwordHash,fullName,email,phone", lines);
}

void Persistence::saveBookings(const DataStore& store) {
    std::vector<std::string> lines;
    for (const auto& b : store.bookings()) lines.push_back(b.toCsv());
    writeLines(bookingsPath_, "#id,customerId,vehicleId,start,end,totalCost,status", lines);
}

} // namespace drivesim
