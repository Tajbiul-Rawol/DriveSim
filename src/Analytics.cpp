// =============================================================================
//  DriveSim  -  Analytics.cpp   [ STUB - TO BE IMPLEMENTED ]
//
//  SRS Feature 5: Administrative Analytics
//  Owner: <teammate assigned to Analytics & Reporting>
//
//  Analytics holds a reference to the live DataStore as `store_` (see
//  Analytics.h). Application::showAnalytics() calls dashboard(), which is glued
//  up for you - your job is the four report methods it depends on. Each returns
//  a formatted ASCII-table string.
//
//  Data you will need (all on DataStore):
//    store_.vehicles()  -> owned vehicles (status via v->status())
//    store_.bookings()  -> all bookings   (b.status(), b.totalCost(), b.vehicleId())
//    store_.findVehicle(id) -> look up a vehicle by id (for its category)
//  Revenue counts bookings whose status is Confirmed OR Completed.
//
//  RULES:
//    * Do NOT change any signature in Analytics.h.
//    * Delete the "(void)..." line once you use the parameter.
//    * Build clean with -Wall -Wextra before opening a pull request.
//    * The money()/padRight()/padLeft()/rule() helpers in Common.h handle
//      currency formatting and table alignment.
//  See CONTRIBUTING.md for the full brief and acceptance criteria.
// =============================================================================
#include "Analytics.h"

#include "Common.h"

namespace drivesim {

// Glue - already done. Once the four methods below are implemented, the full
// dashboard renders automatically.
std::string Analytics::dashboard() const {
    return fleetUtilization() + "\n" + revenueByCategory() + "\n"
         + bookingFrequency() + "\n" + topRevenueVehicles();
}

std::string Analytics::fleetUtilization() const {
    // TODO(analytics): count vehicles by status (Available / Rented /
    // Maintenance) from store_.vehicles(); render a table showing each count
    // and its share of the fleet, plus overall utilisation (rented / fleet).
    return "  *** FLEET UTILISATION not implemented yet - see Analytics.cpp ***\n";
}

std::string Analytics::revenueByCategory() const {
    // TODO(analytics): for bookings that are Confirmed or Completed, group
    // totalCost() by the vehicle's category (look the vehicle up by id) and
    // render a table of bookings count, revenue and share per category.
    return "  *** REVENUE BY CATEGORY not implemented yet - see Analytics.cpp ***\n";
}

std::string Analytics::bookingFrequency() const {
    // TODO(analytics): count bookings by status (Confirmed / Completed /
    // Cancelled) and render a table with counts and shares.
    return "  *** BOOKING FREQUENCY not implemented yet - see Analytics.cpp ***\n";
}

std::string Analytics::topRevenueVehicles(int topN) const {
    // TODO(analytics): sum revenue per vehicle (Confirmed/Completed bookings),
    // sort highest-first, and list the top N with their revenue.
    (void)topN;                          // <-- remove when implemented
    return "  *** TOP REVENUE VEHICLES not implemented yet - see Analytics.cpp ***\n";
}

} // namespace drivesim
