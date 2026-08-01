// =============================================================================
//  DriveSim  -  Booking.h
//  A booking links a customer to a vehicle for a rental period and records the
//  charged total and lifecycle status. Stored as a value type in the DataStore.
// =============================================================================
#ifndef DRIVESIM_BOOKING_H
#define DRIVESIM_BOOKING_H

#include <string>

#include "Common.h"

namespace drivesim {

class Booking {
public:
    Booking() = default;
    Booking(std::string id, std::string customerId, std::string vehicleId,
            Date start, Date end, double totalCost,
            BookingStatus status = BookingStatus::Confirmed);

    const std::string& id()         const { return id_; }
    const std::string& customerId() const { return customerId_; }
    const std::string& vehicleId()  const { return vehicleId_; }
    const Date&        startDate()  const { return start_; }
    const Date&        endDate()    const { return end_; }
    double             totalCost()  const { return totalCost_; }
    BookingStatus      status()     const { return status_; }

    // Whole days of hire (end - start); always at least 1.
    int  durationDays() const;
    void setStatus(BookingStatus s) { status_ = s; }
    bool isActive()     const { return status_ == BookingStatus::Confirmed; }

    // Column order: id, customerId, vehicleId, start, end, totalCost, status
    std::string toCsv() const;
    static bool fromCsv(const std::string& line, Booking& out);

private:
    std::string   id_;
    std::string   customerId_;
    std::string   vehicleId_;
    Date          start_;
    Date          end_;
    double        totalCost_ = 0.0;
    BookingStatus status_    = BookingStatus::Confirmed;
};

} // namespace drivesim

#endif // DRIVESIM_BOOKING_H
