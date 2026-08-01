// =============================================================================
//  DriveSim  -  DataStore.h
//  In-memory repository. Owns all domain objects (vehicles and users via
//  unique_ptr; bookings by value) and exposes lookup, mutation and query
//  helpers. Persistence and analytics operate against this single store,
//  keeping the domain model independent of how it is stored or presented.
// =============================================================================
#ifndef DRIVESIM_DATASTORE_H
#define DRIVESIM_DATASTORE_H

#include <memory>
#include <string>
#include <vector>

#include "Booking.h"
#include "User.h"
#include "Vehicle.h"

namespace drivesim {

class DataStore {
public:
    // ---- Vehicles --------------------------------------------------------
    void     addVehicle(std::unique_ptr<Vehicle> v);
    Vehicle* findVehicle(const std::string& id);
    bool     removeVehicle(const std::string& id);
    const std::vector<std::unique_ptr<Vehicle>>& vehicles() const { return vehicles_; }

    std::vector<Vehicle*> allVehicles();
    std::vector<Vehicle*> availableVehicles();
    std::vector<Vehicle*> vehiclesByCategory(const std::string& category);
    std::vector<Vehicle*> vehiclesUnderMaxRate(double maxEffectiveRate);

    // ---- Users -----------------------------------------------------------
    void  addUser(std::unique_ptr<User> u);
    User* findUserByUsername(const std::string& username);
    User* findUserById(const std::string& id);
    const std::vector<std::unique_ptr<User>>& users() const { return users_; }
    std::vector<Customer*> customers();

    // ---- Bookings --------------------------------------------------------
    void      addBooking(const Booking& b);
    Booking*  findBooking(const std::string& id);
    std::vector<Booking*> bookingsForCustomer(const std::string& customerId);
    std::vector<Booking*> bookingsForVehicle(const std::string& vehicleId);
    std::vector<Booking>& bookings()             { return bookings_; }
    const std::vector<Booking>& bookings() const { return bookings_; }

    // ---- ID generation ---------------------------------------------------
    std::string nextVehicleId();
    std::string nextCustomerId();
    std::string nextBookingId();

private:
    std::vector<std::unique_ptr<Vehicle>> vehicles_;
    std::vector<std::unique_ptr<User>>    users_;
    std::vector<Booking>                  bookings_;
};

} // namespace drivesim

#endif // DRIVESIM_DATASTORE_H
