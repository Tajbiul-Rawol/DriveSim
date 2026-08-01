// =============================================================================
//  DriveSim  -  Analytics.h
//  Administrative reporting rendered as formatted ASCII tables from the live
//  data store: fleet utilisation, revenue by category, booking frequency and
//  the highest-revenue vehicles.
// =============================================================================
#ifndef DRIVESIM_ANALYTICS_H
#define DRIVESIM_ANALYTICS_H

#include <string>

#include "DataStore.h"

namespace drivesim {

class Analytics {
public:
    explicit Analytics(DataStore& store) : store_(store) {}

    std::string dashboard() const;          // all reports, one after another

    std::string fleetUtilization() const;
    std::string revenueByCategory() const;
    std::string bookingFrequency() const;
    std::string topRevenueVehicles(int topN = 5) const;

private:
    DataStore& store_;
};

} // namespace drivesim

#endif // DRIVESIM_ANALYTICS_H
