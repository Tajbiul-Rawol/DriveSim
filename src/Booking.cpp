// =============================================================================
//  DriveSim  -  Booking.cpp
// =============================================================================
#include "Booking.h"

namespace drivesim {

Booking::Booking(std::string id, std::string customerId, std::string vehicleId,
                 Date start, Date end, double totalCost, BookingStatus status)
    : id_(std::move(id)), customerId_(std::move(customerId)),
      vehicleId_(std::move(vehicleId)), start_(start), end_(end),
      totalCost_(totalCost), status_(status) {}

int Booking::durationDays() const {
    long d = start_.daysUntil(end_);
    return d < 1 ? 1 : static_cast<int>(d);
}

std::string Booking::toCsv() const {
    return makeCsvLine({ id_, customerId_, vehicleId_,
                         start_.toString(), end_.toString(),
                         plainNumber(totalCost_),
                         toString(status_) });
}

bool Booking::fromCsv(const std::string& line, Booking& out) {
    auto f = parseCsvLine(line);
    if (f.size() < 7) return false;
    Date s, e;
    if (!Date::parse(f[3], s)) return false;
    if (!Date::parse(f[4], e)) return false;
    double cost = 0.0;
    try { cost = std::stod(trim(f[5])); } catch (...) { return false; }
    BookingStatus st = BookingStatus::Confirmed;
    bookingStatusFromString(f[6], st);
    out = Booking(f[0], f[1], f[2], s, e, cost, st);
    return true;
}

} // namespace drivesim
