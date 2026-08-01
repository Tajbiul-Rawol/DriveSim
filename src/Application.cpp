// =============================================================================
//  DriveSim  -  Application.cpp
// =============================================================================
#include "Application.h"

#include <iostream>
#include <memory>

#include "Analytics.h"

namespace drivesim {

Application::Application()
    : store_(), persistence_("data"), pricing_(25.00, 0.10), io_(std::cin, std::cout) {}

// ---------------------------------------------------------------------------
//  Entry point
// ---------------------------------------------------------------------------
int Application::run() {
    try {
        persistence_.load(store_);
    } catch (const std::exception& e) {
        std::cerr << "Failed to load data: " << e.what() << "\n";
    }

    printBanner();

    try {
        mainMenu();
    } catch (const InputClosed&) {
        std::cout << "\n[input closed] Saving and exiting.\n";
    }

    persist();
    std::cout << "\nData saved to '" << persistence_.dataDir() << "/'. Goodbye.\n";
    return 0;
}

void Application::printBanner() {
    std::cout <<
        "\n" << rule(56, '=') << "\n"
        "                 D R I V E   S I M\n"
        "      Terminal Fleet & Rental Management System\n"
        << rule(56, '=') << "\n"
        "  Default admin login:  admin / admin123\n"
        "  Demo customer login:  demo  / demo123\n";
}

// ---------------------------------------------------------------------------
//  Top-level menu
// ---------------------------------------------------------------------------
void Application::mainMenu() {
    while (true) {
        std::cout << "\n" << rule(30, '-') << "\n"
                  << "  DriveSim - Main Menu\n"
                  << rule(30, '-') << "\n"
                  << "  1) Log in\n"
                  << "  2) Register (new customer)\n"
                  << "  3) Exit\n";
        int choice = io_.readIntInRange("Select an option: ", 1, 3);
        switch (choice) {
            case 1: doLogin();    break;
            case 2: doRegister(); break;
            case 3: return;
        }
    }
}

void Application::doRegister() {
    std::cout << "\n-- Register New Customer --\n";
    std::string username = io_.readNonEmpty("Choose a username: ");
    if (store_.findUserByUsername(username)) {
        std::cout << "  [!] That username is already taken.\n";
        return;
    }
    std::string password = io_.readPassword("Choose a password: ");
    std::string name     = io_.readNonEmpty("Full name: ");
    std::string email    = io_.readNonEmpty("Email: ");
    std::string phone    = io_.readLine("Phone (optional): ");

    std::string id = store_.nextCustomerId();
    store_.addUser(std::make_unique<Customer>(
        id, username, simpleHash(password), name, email, phone));
    persist();
    std::cout << "  [OK] Account " << id << " created. You can now log in.\n";
}

void Application::doLogin() {
    std::cout << "\n-- Log In --\n";
    std::string username = io_.readNonEmpty("Username: ");
    std::string password = io_.readPassword("Password: ");

    User* user = store_.findUserByUsername(username);
    if (!user || !user->checkPassword(password)) {
        std::cout << "  [!] Invalid username or password.\n";
        return;
    }
    std::cout << "  Welcome, " << user->fullName() << "!\n";

    if (user->role() == UserRole::Admin) {
        adminMenu(*static_cast<Admin*>(user));
    } else {
        customerMenu(*static_cast<Customer*>(user));
    }
}

// ---------------------------------------------------------------------------
//  Customer flows
// ---------------------------------------------------------------------------
void Application::customerMenu(Customer& c) {
    while (true) {
        std::cout << "\n" << rule(36, '-') << "\n"
                  << "  Customer Menu - " << c.fullName() << "\n"
                  << rule(36, '-') << "\n"
                  << "  1) Browse vehicles\n"
                  << "  2) Create a booking\n"
                  << "  3) View my bookings\n"
                  << "  4) Cancel a booking\n"
                  << "  5) Log out\n";
        int choice = io_.readIntInRange("Select: ", 1, 5);
        switch (choice) {
            case 1: browseVehicles();  break;
            case 2: createBooking(c);  break;
            case 3: viewMyBookings(c); break;
            case 4: cancelBooking(c);  break;
            case 5: std::cout << "  Logged out.\n"; return;
        }
    }
}

void Application::browseVehicles() {
    std::cout << "\n-- Browse Vehicles --\n"
              << "  1) All vehicles\n"
              << "  2) By category\n"
              << "  3) Available only\n"
              << "  4) Under a maximum daily rate\n";
    int choice = io_.readIntInRange("View: ", 1, 4);

    std::vector<Vehicle*> list;
    switch (choice) {
        case 1: list = store_.allVehicles(); break;
        case 2: {
            auto cats = Vehicle::categories();
            for (std::size_t i = 0; i < cats.size(); ++i)
                std::cout << "  " << (i + 1) << ") " << cats[i] << "\n";
            int ci = io_.readIntInRange("Category: ", 1, static_cast<int>(cats.size()));
            list = store_.vehiclesByCategory(cats[ci - 1]);
            break;
        }
        case 3: list = store_.availableVehicles(); break;
        case 4: {
            double mx = io_.readDouble("Maximum daily rate (AUD): ", 1.0, 100000.0);
            list = store_.vehiclesUnderMaxRate(mx);
            break;
        }
    }
    printVehicleTable(list, false);
}

void Application::createBooking(Customer& c) {
    std::cout << "\n-- Create a Booking --\n";
    auto available = store_.availableVehicles();
    if (available.empty()) {
        std::cout << "  Sorry, no vehicles are currently available.\n";
        return;
    }
    printVehicleTable(available, false);

    std::string vid = io_.readNonEmpty("Enter the Vehicle ID to book (or 0 to cancel): ");
    if (vid == "0") return;

    Vehicle* v = store_.findVehicle(vid);
    if (!v) {
        std::cout << "  [!] No vehicle with ID '" << vid << "'.\n";
        return;
    }
    if (v->status() != VehicleStatus::Available) {
        std::cout << "  [!] " << v->id() << " is currently " << toString(v->status())
                  << " and cannot be booked.\n";
        return;
    }

    Date today = Date::today();
    Date start = io_.readFutureDate("Pick-up date (YYYY-MM-DD): ", today);
    Date end   = io_.readFutureDate("Return date  (YYYY-MM-DD): ", start);
    if (!(end > start)) {
        std::cout << "  [!] Return date must be after the pick-up date.\n";
        return;
    }

    int days = static_cast<int>(start.daysUntil(end));
    PriceBreakdown pb = pricing_.quote(*v, days);

    std::string bookingId = store_.nextBookingId();
    Booking booking(bookingId, c.id(), v->id(), start, end, pb.total,
                    BookingStatus::Confirmed);

    std::cout << "\n" << InvoiceRenderer::invoice(booking, *v, c, pb) << "\n\n";

    if (!io_.readYesNo("Confirm this booking? (y/n): ")) {
        std::cout << "  Booking abandoned - nothing was charged.\n";
        return;
    }

    // Enforce the state machine: Available -> Rented.
    if (!v->transitionTo(VehicleStatus::Rented)) {
        std::cout << "  [!] Could not reserve the vehicle (its state changed).\n";
        return;
    }
    store_.addBooking(booking);
    persist();
    std::cout << "  [OK] Booking " << bookingId << " confirmed. Enjoy your trip!\n";
}

void Application::viewMyBookings(Customer& c) {
    std::cout << "\n-- My Bookings --\n";
    auto list = store_.bookingsForCustomer(c.id());
    if (list.empty()) {
        std::cout << "  You have no bookings yet.\n";
        return;
    }
    std::cout << "  " << padRight("Booking", 9) << padRight("Vehicle", 9)
              << padRight("From", 12) << padRight("To", 12)
              << padLeft("Total", 12) << "  " << padRight("Status", 10) << "\n";
    std::cout << "  " << rule(66, '-') << "\n";
    for (auto* b : list) {
        std::cout << "  " << padRight(b->id(), 9) << padRight(b->vehicleId(), 9)
                  << padRight(b->startDate().toString(), 12)
                  << padRight(b->endDate().toString(), 12)
                  << padLeft(money(b->totalCost()), 12) << "  "
                  << padRight(toString(b->status()), 10) << "\n";
    }
}

void Application::cancelBooking(Customer& c) {
    std::cout << "\n-- Cancel a Booking --\n";
    std::vector<Booking*> active;
    for (auto* b : store_.bookingsForCustomer(c.id()))
        if (b->isActive()) active.push_back(b);

    if (active.empty()) {
        std::cout << "  You have no active bookings to cancel.\n";
        return;
    }
    for (auto* b : active) {
        std::cout << "  " << padRight(b->id(), 9) << padRight(b->vehicleId(), 9)
                  << b->startDate().toString() << " -> " << b->endDate().toString()
                  << "   " << money(b->totalCost()) << "\n";
    }

    std::string bid = io_.readNonEmpty("Enter Booking ID to cancel (or 0 to abort): ");
    if (bid == "0") return;

    Booking* target = nullptr;
    for (auto* b : active)
        if (b->id() == bid) { target = b; break; }
    if (!target) {
        std::cout << "  [!] No active booking with ID '" << bid << "'.\n";
        return;
    }
    if (!io_.readYesNo("Really cancel " + bid + "? (y/n): ")) return;

    target->setStatus(BookingStatus::Cancelled);
    Vehicle* v = store_.findVehicle(target->vehicleId());
    if (v) v->transitionTo(VehicleStatus::Available);
    persist();

    if (v)
        std::cout << "\n" << InvoiceRenderer::cancellationReceipt(*target, *v, c) << "\n";
    std::cout << "\n  [OK] Booking " << bid << " cancelled.\n";
}

// ---------------------------------------------------------------------------
//  Admin flows
// ---------------------------------------------------------------------------
void Application::adminMenu(Admin& a) {
    while (true) {
        std::cout << "\n" << rule(36, '=') << "\n"
                  << "  Admin Console - " << a.fullName() << "\n"
                  << rule(36, '=') << "\n"
                  << "   1) View fleet\n"
                  << "   2) Add vehicle\n"
                  << "   3) Update vehicle rate\n"
                  << "   4) Maintenance (set / clear)\n"
                  << "   5) Process a vehicle return\n"
                  << "   6) Remove vehicle\n"
                  << "   7) View all bookings\n"
                  << "   8) View customers\n"
                  << "   9) Analytics dashboard\n"
                  << "  10) Log out\n";
        int choice = io_.readIntInRange("Select: ", 1, 10);
        switch (choice) {
            case 1:  viewFleet();         break;
            case 2:  addVehicle();        break;
            case 3:  updateVehicle();     break;
            case 4:  manageMaintenance(); break;
            case 5:  processReturn();     break;
            case 6:  removeVehicle();     break;
            case 7:  viewAllBookings();   break;
            case 8:  viewCustomers();     break;
            case 9:  showAnalytics();     break;
            case 10: std::cout << "  Logged out.\n"; return;
        }
    }
}

void Application::viewFleet() {
    std::cout << "\n-- Fleet Manifest --\n";
    printVehicleTable(store_.allVehicles(), true);
}

void Application::addVehicle() {
    std::cout << "\n-- Add Vehicle --\n";
    auto cats = Vehicle::categories();
    for (std::size_t i = 0; i < cats.size(); ++i)
        std::cout << "  " << (i + 1) << ") " << cats[i] << "\n";
    int ci = io_.readIntInRange("Choose a category: ", 1, static_cast<int>(cats.size()));
    std::string category = cats[ci - 1];

    std::string make  = io_.readNonEmpty("Make (e.g. Toyota): ");
    std::string model = io_.readNonEmpty("Model (e.g. Corolla): ");
    int    year = io_.readIntInRange("Year: ", 1980, 2100);
    double rate = io_.readDouble("Base daily rate (AUD): ", 1.0, 100000.0);

    std::string id = store_.nextVehicleId();
    store_.addVehicle(Vehicle::create(category, id, make, model, year, rate));
    persist();

    Vehicle* v = store_.findVehicle(id);
    std::cout << "  [OK] Added " << id << " - " << (v ? v->displayName() : "")
              << " (effective " << (v ? money(v->effectiveDailyRate()) : "") << "/day).\n";
}

void Application::updateVehicle() {
    std::cout << "\n-- Update Vehicle Rate --\n";
    std::string id = io_.readNonEmpty("Vehicle ID to update: ");
    Vehicle* v = store_.findVehicle(id);
    if (!v) {
        std::cout << "  [!] No vehicle with ID '" << id << "'.\n";
        return;
    }
    std::cout << "  " << v->id() << " - " << v->displayName()
              << " | base " << money(v->baseDailyRate())
              << " | status " << toString(v->status()) << "\n";
    double rate = io_.readDouble("New base daily rate (AUD): ", 1.0, 100000.0);
    v->setBaseDailyRate(rate);
    persist();
    std::cout << "  [OK] Base rate updated. New effective rate: "
              << money(v->effectiveDailyRate()) << "/day.\n";
}

void Application::manageMaintenance() {
    std::cout << "\n-- Maintenance --\n";
    std::string id = io_.readNonEmpty("Vehicle ID: ");
    Vehicle* v = store_.findVehicle(id);
    if (!v) {
        std::cout << "  [!] No vehicle with ID '" << id << "'.\n";
        return;
    }
    std::cout << "  Current status: " << toString(v->status()) << "\n";

    if (v->status() == VehicleStatus::Rented) {
        std::cout << "  [!] Vehicle is rented. It must be returned before servicing.\n";
        return;
    }
    if (v->status() == VehicleStatus::Maintenance) {
        if (io_.readYesNo("Return this vehicle to service? (y/n): ")) {
            v->transitionTo(VehicleStatus::Available);
            persist();
            std::cout << "  [OK] " << id << " is now Available.\n";
        }
    } else {  // Available
        if (io_.readYesNo("Place this vehicle into maintenance? (y/n): ")) {
            v->transitionTo(VehicleStatus::Maintenance);
            persist();
            std::cout << "  [OK] " << id << " is now under maintenance.\n";
        }
    }
}

void Application::processReturn() {
    std::cout << "\n-- Process Vehicle Return --\n";
    std::string id = io_.readNonEmpty("Vehicle ID being returned: ");
    Vehicle* v = store_.findVehicle(id);
    if (!v) {
        std::cout << "  [!] No vehicle with ID '" << id << "'.\n";
        return;
    }
    if (v->status() != VehicleStatus::Rented) {
        std::cout << "  [!] " << id << " is not currently rented.\n";
        return;
    }
    Booking* active = nullptr;
    for (auto& b : store_.bookings())
        if (b.vehicleId() == id && b.isActive()) { active = &b; break; }
    if (active) active->setStatus(BookingStatus::Completed);
    v->transitionTo(VehicleStatus::Available);
    persist();
    std::cout << "  [OK] " << id << " returned"
              << (active ? " (booking " + active->id() + " completed)" : "")
              << ". Now Available.\n";
}

void Application::removeVehicle() {
    std::cout << "\n-- Remove Vehicle --\n";
    std::string id = io_.readNonEmpty("Vehicle ID to remove: ");
    Vehicle* v = store_.findVehicle(id);
    if (!v) {
        std::cout << "  [!] No vehicle with ID '" << id << "'.\n";
        return;
    }
    if (v->status() == VehicleStatus::Rented) {
        std::cout << "  [!] Cannot remove a rented vehicle. Process its return first.\n";
        return;
    }
    std::cout << "  About to remove " << v->id() << " - " << v->displayName() << ".\n";
    if (!io_.readYesNo("Are you sure? (y/n): ")) return;
    store_.removeVehicle(id);
    persist();
    std::cout << "  [OK] Vehicle " << id << " removed from the fleet.\n";
}

void Application::viewAllBookings() {
    std::cout << "\n-- All Bookings --\n";
    auto& all = store_.bookings();
    if (all.empty()) {
        std::cout << "  No bookings recorded.\n";
        return;
    }
    std::cout << "  " << padRight("Booking", 9) << padRight("Customer", 10)
              << padRight("Vehicle", 9) << padRight("From", 12) << padRight("To", 12)
              << padLeft("Total", 11) << "  " << padRight("Status", 10) << "\n";
    std::cout << "  " << rule(76, '-') << "\n";
    for (auto& b : all) {
        std::cout << "  " << padRight(b.id(), 9) << padRight(b.customerId(), 10)
                  << padRight(b.vehicleId(), 9) << padRight(b.startDate().toString(), 12)
                  << padRight(b.endDate().toString(), 12)
                  << padLeft(money(b.totalCost()), 11) << "  "
                  << padRight(toString(b.status()), 10) << "\n";
    }
}

void Application::viewCustomers() {
    std::cout << "\n-- Registered Customers --\n";
    auto list = store_.customers();
    if (list.empty()) {
        std::cout << "  No customers registered.\n";
        return;
    }
    std::cout << "  " << padRight("ID", 8) << padRight("Username", 14)
              << padRight("Name", 22) << padRight("Email", 26) << "\n";
    std::cout << "  " << rule(70, '-') << "\n";
    for (auto* c : list) {
        std::cout << "  " << padRight(c->id(), 8) << padRight(c->username(), 14)
                  << padRight(c->fullName(), 22) << padRight(c->email(), 26) << "\n";
    }
}

void Application::showAnalytics() {
    Analytics analytics(store_);
    std::cout << analytics.dashboard() << "\n";
}

// ---------------------------------------------------------------------------
//  Helpers
// ---------------------------------------------------------------------------
void Application::printVehicleTable(const std::vector<Vehicle*>& list, bool admin) {
    if (list.empty()) {
        std::cout << "  (no vehicles to display)\n";
        return;
    }
    if (admin) {
        std::cout << "  " << padRight("ID", 9) << padRight("Category", 11)
                  << padRight("Vehicle", 24) << padLeft("Base/day", 10)
                  << padLeft("Eff/day", 11) << "  " << padRight("Status", 12) << "\n";
        std::cout << "  " << rule(79, '-') << "\n";
        for (auto* v : list) {
            std::cout << "  " << padRight(v->id(), 9) << padRight(v->category(), 11)
                      << padRight(v->displayName(), 24)
                      << padLeft(money(v->baseDailyRate()), 10)
                      << padLeft(money(v->effectiveDailyRate()), 11)
                      << "  " << padRight(toString(v->status()), 12) << "\n";
        }
    } else {
        std::cout << "  " << padRight("ID", 9) << padRight("Category", 11)
                  << padRight("Vehicle", 26) << padLeft("Rate/day", 12)
                  << "  " << padRight("Status", 12) << "\n";
        std::cout << "  " << rule(72, '-') << "\n";
        for (auto* v : list) {
            std::cout << "  " << padRight(v->id(), 9) << padRight(v->category(), 11)
                      << padRight(v->displayName(), 26)
                      << padLeft(money(v->effectiveDailyRate()), 12)
                      << "  " << padRight(toString(v->status()), 12) << "\n";
        }
    }
}

void Application::persist() {
    try {
        persistence_.save(store_);
    } catch (const std::exception& e) {
        std::cerr << "  [warn] Failed to save data: " << e.what() << "\n";
    }
}

} // namespace drivesim
