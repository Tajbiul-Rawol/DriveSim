// =============================================================================
//  DriveSim  -  Persistence.h
//  CSV-backed persistence. Loads the three data files into a DataStore and
//  writes them back out. On first run (files absent) it seeds a default admin
//  account and a starter fleet so the application is immediately usable.
// =============================================================================
#ifndef DRIVESIM_PERSISTENCE_H
#define DRIVESIM_PERSISTENCE_H

#include <string>

#include "DataStore.h"

namespace drivesim {

class Persistence {
public:
    explicit Persistence(std::string dataDir = "data");

    // Populate store from disk. Missing files trigger seeding + a save.
    void load(DataStore& store);

    // Write the entire store to disk (write-to-temp then rename per file).
    void save(const DataStore& store);

    const std::string& dataDir() const { return dataDir_; }

private:
    void ensureDir() const;
    void seed(DataStore& store);

    void loadVehicles(DataStore&);
    void loadUsers(DataStore&);
    void loadBookings(DataStore&);
    void saveVehicles(const DataStore&);
    void saveUsers(const DataStore&);
    void saveBookings(const DataStore&);

    std::string dataDir_;
    std::string vehiclesPath_;
    std::string usersPath_;
    std::string bookingsPath_;
};

} // namespace drivesim

#endif // DRIVESIM_PERSISTENCE_H
