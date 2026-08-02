// =============================================================================
//  DriveSim  -  Analytics.cpp
// =============================================================================
#include "Analytics.h"

#include <algorithm>
#include <map>
#include <sstream>

namespace drivesim {

namespace {
// Revenue-bearing bookings are those that were honoured (Confirmed or
// Completed). Cancelled bookings contribute no revenue.
bool countsAsRevenue(const Booking& b) {
    return b.status() == BookingStatus::Confirmed ||
           b.status() == BookingStatus::Completed;
}

std::string pct(double numerator, double denominator) {
    if (denominator <= 0) return "0.0%";
    double p = 100.0 * numerator / denominator;
    std::ostringstream os;
    os.setf(std::ios::fixed);
    os.precision(1);
    os << p << "%";
    return os.str();
}

std::string header(const std::string& title) {
    std::ostringstream os;
    os << "\n" << rule(64, '=') << "\n  " << title << "\n" << rule(64, '=') << "\n";
    return os.str();
}
} // namespace

std::string Analytics::fleetUtilization() const {
    int total = 0, available = 0, rented = 0, maintenance = 0;
    for (const auto& v : store_.vehicles()) {
        ++total;
        switch (v->status()) {
            case VehicleStatus::Available:   ++available;   break;
            case VehicleStatus::Rented:      ++rented;      break;
            case VehicleStatus::Maintenance: ++maintenance; break;
        }
    }

    std::ostringstream os;
    os << header("FLEET UTILISATION");
    os << "  " << padRight("Status", 16) << padLeft("Count", 8)
       << padLeft("Share", 12) << "\n";
    os << "  " << rule(36, '-') << "\n";
    os << "  " << padRight("Available", 16) << padLeft(std::to_string(available), 8)
       << padLeft(pct(available, total), 12) << "\n";
    os << "  " << padRight("Rented", 16) << padLeft(std::to_string(rented), 8)
       << padLeft(pct(rented, total), 12) << "\n";
    os << "  " << padRight("Maintenance", 16) << padLeft(std::to_string(maintenance), 8)
       << padLeft(pct(maintenance, total), 12) << "\n";
    os << "  " << rule(36, '-') << "\n";
    os << "  " << padRight("Fleet size", 16) << padLeft(std::to_string(total), 8) << "\n";
    os << "  Utilisation (rented / fleet): " << pct(rented, total) << "\n";
    return os.str();
}

std::string Analytics::revenueByCategory() const {
    std::map<std::string, double> revenue;
    std::map<std::string, int>    counts;
    double grand = 0.0;

    for (const auto& b : store_.bookings()) {
        if (!countsAsRevenue(b)) continue;
        auto* v = const_cast<DataStore&>(store_).findVehicle(b.vehicleId());
        std::string cat = v ? v->category() : "(removed)";
        revenue[cat] += b.totalCost();
        counts[cat]  += 1;
        grand        += b.totalCost();
    }

    std::ostringstream os;
    os << header("REVENUE BY CATEGORY");
    os << "  " << padRight("Category", 16) << padLeft("Bookings", 10)
       << padLeft("Revenue", 16) << padLeft("Share", 10) << "\n";
    os << "  " << rule(52, '-') << "\n";
    if (revenue.empty()) {
        os << "  (no revenue-bearing bookings yet)\n";
        return os.str();
    }
    for (const auto& kv : revenue) {
        os << "  " << padRight(kv.first, 16)
           << padLeft(std::to_string(counts[kv.first]), 10)
           << padLeft(money(kv.second), 16)
           << padLeft(pct(kv.second, grand), 10) << "\n";
    }
    os << "  " << rule(52, '-') << "\n";
    os << "  " << padRight("TOTAL", 16) << padLeft("", 10)
       << padLeft(money(grand), 16) << "\n";
    return os.str();
}

std::string Analytics::bookingFrequency() const {
    int confirmed = 0, cancelled = 0, completed = 0;
    for (const auto& b : store_.bookings()) {
        switch (b.status()) {
            case BookingStatus::Confirmed: ++confirmed; break;
            case BookingStatus::Cancelled: ++cancelled; break;
            case BookingStatus::Completed: ++completed; break;
        }
    }
    int total = confirmed + cancelled + completed;

    std::ostringstream os;
    os << header("BOOKING FREQUENCY");
    os << "  " << padRight("Booking status", 18) << padLeft("Count", 8)
       << padLeft("Share", 12) << "\n";
    os << "  " << rule(38, '-') << "\n";
    os << "  " << padRight("Confirmed (active)", 18) << padLeft(std::to_string(confirmed), 8)
       << padLeft(pct(confirmed, total), 12) << "\n";
    os << "  " << padRight("Completed", 18) << padLeft(std::to_string(completed), 8)
       << padLeft(pct(completed, total), 12) << "\n";
    os << "  " << padRight("Cancelled", 18) << padLeft(std::to_string(cancelled), 8)
       << padLeft(pct(cancelled, total), 12) << "\n";
    os << "  " << rule(38, '-') << "\n";
    os << "  " << padRight("Total bookings", 18) << padLeft(std::to_string(total), 8) << "\n";
    return os.str();
}

std::string Analytics::topRevenueVehicles(int topN) const {
    std::map<std::string, double> revenue;   // vehicleId -> revenue
    for (const auto& b : store_.bookings()) {
        if (!countsAsRevenue(b)) continue;
        revenue[b.vehicleId()] += b.totalCost();
    }

    std::vector<std::pair<std::string, double>> ranked(revenue.begin(), revenue.end());
    std::sort(ranked.begin(), ranked.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    std::ostringstream os;
    os << header("TOP REVENUE-GENERATING VEHICLES");
    os << "  " << padRight("#", 4) << padRight("Vehicle", 34)
       << padLeft("Revenue", 16) << "\n";
    os << "  " << rule(54, '-') << "\n";
    if (ranked.empty()) {
        os << "  (no revenue-bearing bookings yet)\n";
        return os.str();
    }
    int rank = 1;
    for (const auto& r : ranked) {
        if (rank > topN) break;
        auto* v = const_cast<DataStore&>(store_).findVehicle(r.first);
        std::string label = v ? (v->id() + "  " + v->displayName())
                              : (r.first + "  (removed)");
        os << "  " << padRight(std::to_string(rank), 4) << padRight(label, 34)
           << padLeft(money(r.second), 16) << "\n";
        ++rank;
    }
    return os.str();
}

std::string Analytics::dashboard() const {
    std::ostringstream os;
    os << fleetUtilization()
       << revenueByCategory()
       << bookingFrequency()
       << topRevenueVehicles();
    return os.str();
}

} // namespace drivesim
