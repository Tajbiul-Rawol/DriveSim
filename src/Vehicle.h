// =============================================================================
//  DriveSim  -  Vehicle.h
//  Abstract Vehicle base + concrete categories (Sedan, SUV, ElectricVehicle,
//  LuxuryVan). Concrete types supply their category name and pricing
//  multiplier, so billing and analytics work polymorphically. Adding a new
//  category is a matter of deriving a new class - the pricing engine and UI
//  need not change (Open/Closed Principle).
//
//  The Available / Rented / Maintenance status is governed by an explicit
//  state machine (isTransitionAllowed / transitionTo) so illegal transitions
//  such as booking a vehicle already under maintenance are rejected in one
//  authoritative place.
// =============================================================================
#ifndef DRIVESIM_VEHICLE_H
#define DRIVESIM_VEHICLE_H

#include <memory>
#include <string>

#include "Common.h"

namespace drivesim {

class Vehicle {
public:
    Vehicle(std::string id, std::string make, std::string model, int year,
            double baseDailyRate, VehicleStatus status = VehicleStatus::Available);
    virtual ~Vehicle() = default;

    // ---- Polymorphic interface -------------------------------------------
    virtual std::string category() const = 0;        // "Sedan", "SUV", ...
    virtual double      categoryMultiplier() const = 0;
    virtual std::string features() const;            // human-readable extras

    // ---- State machine ---------------------------------------------------
    static bool isTransitionAllowed(VehicleStatus from, VehicleStatus to);
    // Attempts the transition; returns false and leaves state unchanged if the
    // transition is not permitted.
    bool transitionTo(VehicleStatus target);

    // ---- Accessors -------------------------------------------------------
    const std::string& id()    const { return id_; }
    const std::string& make()  const { return make_; }
    const std::string& model() const { return model_; }
    int                year()  const { return year_; }
    double baseDailyRate()     const { return baseDailyRate_; }
    VehicleStatus status()     const { return status_; }

    void setBaseDailyRate(double rate) { baseDailyRate_ = rate; }
    // Bypasses the state machine - used only by the CSV loader when
    // reconstructing persisted state.
    void setStatusRaw(VehicleStatus s) { status_ = s; }

    double      effectiveDailyRate() const { return baseDailyRate_ * categoryMultiplier(); }
    std::string displayName() const;                 // "2022 Toyota Camry"

    // ---- Serialisation ---------------------------------------------------
    std::string toCsv() const;
    static std::unique_ptr<Vehicle> fromCsv(const std::string& line);

    // Build the correct concrete type from a category keyword.
    static std::unique_ptr<Vehicle> create(const std::string& category,
                                           const std::string& id,
                                           const std::string& make,
                                           const std::string& model,
                                           int year, double baseDailyRate,
                                           VehicleStatus status = VehicleStatus::Available);

    // Categories the admin "add vehicle" flow may choose from.
    static std::vector<std::string> categories();

protected:
    std::string   id_;
    std::string   make_;
    std::string   model_;
    int           year_;
    double        baseDailyRate_;
    VehicleStatus status_;
};

// ---- Concrete categories --------------------------------------------------

class Sedan : public Vehicle {
public:
    using Vehicle::Vehicle;
    std::string category() const override { return "Sedan"; }
    double      categoryMultiplier() const override { return 1.00; }
    std::string features() const override;
};

class SUV : public Vehicle {
public:
    using Vehicle::Vehicle;
    std::string category() const override { return "SUV"; }
    double      categoryMultiplier() const override { return 1.35; }
    std::string features() const override;
};

class ElectricVehicle : public Vehicle {
public:
    using Vehicle::Vehicle;
    std::string category() const override { return "Electric"; }
    double      categoryMultiplier() const override { return 1.20; }
    std::string features() const override;
};

class LuxuryVan : public Vehicle {
public:
    using Vehicle::Vehicle;
    std::string category() const override { return "LuxuryVan"; }
    double      categoryMultiplier() const override { return 1.80; }
    std::string features() const override;
};

} // namespace drivesim

#endif // DRIVESIM_VEHICLE_H
