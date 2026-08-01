// =============================================================================
//  DriveSim  -  Billing.h
//  Dynamic pricing and invoice generation.
//
//  Cost is derived from the vehicle's effective daily rate (base rate x
//  category multiplier), the rental duration, a flat service fee and 10% GST.
//  Because the category multiplier is polymorphic on Vehicle, the pricing
//  engine needs no knowledge of concrete categories - a new vehicle type is
//  priced correctly without changing this module.
// =============================================================================
#ifndef DRIVESIM_BILLING_H
#define DRIVESIM_BILLING_H

#include <string>

#include "Booking.h"
#include "User.h"
#include "Vehicle.h"

namespace drivesim {

struct PriceBreakdown {
    int    days          = 0;
    double dailyRate     = 0.0;   // effective (base x multiplier)
    double rentalSubtotal= 0.0;   // dailyRate x days
    double serviceFee    = 0.0;
    double taxableAmount = 0.0;   // subtotal + fee
    double gst           = 0.0;   // taxableAmount x gstRate
    double total         = 0.0;   // taxableAmount + gst
};

class PricingEngine {
public:
    explicit PricingEngine(double serviceFee = 25.00, double gstRate = 0.10)
        : serviceFee_(serviceFee), gstRate_(gstRate) {}

    PriceBreakdown quote(const Vehicle& v, int days) const;
    PriceBreakdown quote(double effectiveDailyRate, int days) const;

    double serviceFee() const { return serviceFee_; }
    double gstRate()    const { return gstRate_; }

private:
    double serviceFee_;
    double gstRate_;
};

// Renders formatted, human-readable invoices / receipts to strings so the UI
// layer stays free of formatting detail.
class InvoiceRenderer {
public:
    static std::string invoice(const Booking& b, const Vehicle& v,
                               const User& customer, const PriceBreakdown& pb);
    static std::string cancellationReceipt(const Booking& b, const Vehicle& v,
                                           const User& customer);
};

} // namespace drivesim

#endif // DRIVESIM_BILLING_H
