// =============================================================================
//  DriveSim  -  Application.h
//  The presentation / control layer. Owns the data store, persistence, pricing
//  engine and input reader, and drives the menu-based user interface for both
//  customer and administrator roles. All formatting and flow control lives
//  here; the domain and service modules remain UI-agnostic.
// =============================================================================
#ifndef DRIVESIM_APPLICATION_H
#define DRIVESIM_APPLICATION_H

#include <vector>

#include "Billing.h"
#include "DataStore.h"
#include "InputValidator.h"
#include "Persistence.h"

namespace drivesim {

class Application {
public:
    Application();
    int run();                      // process entry point; returns exit code

private:
    // ---- Top-level flows ----
    void printBanner();
    void mainMenu();
    void doRegister();
    void doLogin();

    // ---- Customer flows ----
    void customerMenu(Customer& c);
    void browseVehicles();
    void createBooking(Customer& c);
    void viewMyBookings(Customer& c);
    void cancelBooking(Customer& c);

    // ---- Admin flows ----
    void adminMenu(Admin& a);
    void viewFleet();
    void addVehicle();
    void updateVehicle();
    void manageMaintenance();
    void processReturn();
    void removeVehicle();
    void viewAllBookings();
    void viewCustomers();
    void showAnalytics();

    // ---- Helpers ----
    void printVehicleTable(const std::vector<Vehicle*>& list, bool admin);
    void persist();

    DataStore     store_;
    Persistence   persistence_;
    PricingEngine pricing_;
    InputReader   io_;
};

} // namespace drivesim

#endif // DRIVESIM_APPLICATION_H
