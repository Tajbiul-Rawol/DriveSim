// =============================================================================
//  DriveSim  -  Vehicle.cpp
// =============================================================================
#include "Vehicle.h"

namespace drivesim {

Vehicle::Vehicle(std::string id, std::string make, std::string model, int year,
                 double baseDailyRate, VehicleStatus status)
    : id_(std::move(id)), make_(std::move(make)), model_(std::move(model)),
      year_(year), baseDailyRate_(baseDailyRate), status_(status) {}

std::string Vehicle::features() const { return "Standard inclusions"; }

// ---- State machine --------------------------------------------------------
//   Available  -> Rented        (a booking is confirmed)
//   Available  -> Maintenance   (taken out of service by an admin)
//   Rented     -> Available     (returned / booking cancelled)
//   Maintenance-> Available     (returned to service)
// Any other transition (e.g. Rented -> Maintenance, or Maintenance -> Rented)
// is rejected: a rented vehicle must be returned before it can be serviced.
bool Vehicle::isTransitionAllowed(VehicleStatus from, VehicleStatus to) {
    if (from == to) return true;                       // idempotent no-op
    switch (from) {
        case VehicleStatus::Available:
            return to == VehicleStatus::Rented || to == VehicleStatus::Maintenance;
        case VehicleStatus::Rented:
            return to == VehicleStatus::Available;
        case VehicleStatus::Maintenance:
            return to == VehicleStatus::Available;
    }
    return false;
}

bool Vehicle::transitionTo(VehicleStatus target) {
    if (!isTransitionAllowed(status_, target)) return false;
    status_ = target;
    return true;
}

std::string Vehicle::displayName() const {
    return std::to_string(year_) + " " + make_ + " " + model_;
}

// ---- Serialisation --------------------------------------------------------
// Column order: id, category, make, model, year, baseDailyRate, status
std::string Vehicle::toCsv() const {
    return makeCsvLine({ id_, category(), make_, model_,
                         std::to_string(year_),
                         plainNumber(baseDailyRate_),
                         toString(status_) });
}

std::unique_ptr<Vehicle> Vehicle::fromCsv(const std::string& line) {
    auto f = parseCsvLine(line);
    if (f.size() < 7) return nullptr;
    try {
        int    year = std::stoi(trim(f[4]));
        double rate = std::stod(trim(f[5]));
        VehicleStatus st = VehicleStatus::Available;
        vehicleStatusFromString(f[6], st);
        return create(f[1], f[0], f[2], f[3], year, rate, st);
    } catch (...) {
        return nullptr;                                // skip a corrupt record
    }
}

std::unique_ptr<Vehicle> Vehicle::create(const std::string& category,
                                         const std::string& id,
                                         const std::string& make,
                                         const std::string& model,
                                         int year, double baseDailyRate,
                                         VehicleStatus status) {
    std::string c = toLower(trim(category));
    if (c == "sedan")
        return std::make_unique<Sedan>(id, make, model, year, baseDailyRate, status);
    if (c == "suv")
        return std::make_unique<SUV>(id, make, model, year, baseDailyRate, status);
    if (c == "electric" || c == "ev" || c == "electricvehicle")
        return std::make_unique<ElectricVehicle>(id, make, model, year, baseDailyRate, status);
    if (c == "luxuryvan" || c == "van" || c == "luxury")
        return std::make_unique<LuxuryVan>(id, make, model, year, baseDailyRate, status);
    return nullptr;
}

std::vector<std::string> Vehicle::categories() {
    return { "Sedan", "SUV", "Electric", "LuxuryVan" };
}

// ---- Category-specific feature blurbs -------------------------------------
std::string Sedan::features() const {
    return "5 seats, air-con, automatic, fuel-efficient";
}
std::string SUV::features() const {
    return "7 seats, AWD, roof rails, elevated ride height";
}
std::string ElectricVehicle::features() const {
    return "Zero emissions, ~450km range, charging cable included";
}
std::string LuxuryVan::features() const {
    return "12 seats, leather trim, climate zones, premium audio";
}

} // namespace drivesim
