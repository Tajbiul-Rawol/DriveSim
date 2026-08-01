// =============================================================================
//  DriveSim  -  Billing.cpp   [ STUB - TO BE IMPLEMENTED ]
//
//  SRS Feature 4: Dynamic Pricing & Invoice Generation
//  Owner: <teammate assigned to Pricing & Invoicing>
//
//  The declarations in Billing.h are the contract the rest of the application
//  already depends on - Application::createBooking() calls PricingEngine::quote()
//  and InvoiceRenderer::invoice(), and cancelBooking() calls
//  cancellationReceipt(). Your job is to fill in the four bodies below.
//
//  RULES:
//    * Do NOT change any signature in Billing.h.
//    * Delete the "(void)..." lines as you start using each parameter.
//    * Build clean with -Wall -Wextra before opening a pull request.
//  See CONTRIBUTING.md for the full brief and acceptance criteria.
// =============================================================================
#include "Billing.h"

#include "Common.h"   // money(), padRight(), padLeft(), rule()

namespace drivesim {

// ---------------------------------------------------------------------------
//  PricingEngine
// ---------------------------------------------------------------------------
PriceBreakdown PricingEngine::quote(const Vehicle& v, int days) const {
    // TODO(pricing): build the full breakdown, then return it.
    //   dailyRate      = v.effectiveDailyRate();     // base x category multiplier
    //   rentalSubtotal = dailyRate * days;
    //   taxableAmount  = rentalSubtotal + serviceFee_;
    //   gst            = taxableAmount * gstRate_;
    //   total          = taxableAmount + gst;
    // Set every field of the PriceBreakdown (days, dailyRate, rentalSubtotal,
    // serviceFee, taxableAmount, gst, total) before returning.
    (void)v; (void)days;                 // <-- remove when implemented
    return PriceBreakdown{};             // placeholder: all fields zero
}

PriceBreakdown PricingEngine::quote(double effectiveDailyRate, int days) const {
    // TODO(pricing): same calculation as above, but starting from a raw daily
    // rate instead of a Vehicle. (Tip: you can reuse the logic.)
    (void)effectiveDailyRate; (void)days;
    return PriceBreakdown{};
}

// ---------------------------------------------------------------------------
//  InvoiceRenderer
// ---------------------------------------------------------------------------
std::string InvoiceRenderer::invoice(const Booking& b, const Vehicle& v,
                                     const User& customer, const PriceBreakdown& pb) {
    // TODO(pricing): return a formatted, multi-line tax-invoice string built
    // from the booking, vehicle, customer and price breakdown. Match the layout
    // in the SRS (invoice no, customer, vehicle, rental period, duration, the
    // itemised charges, GST and total). Use money()/padRight()/padLeft()/rule()
    // from Common.h for alignment. Return the string; the caller prints it.
    (void)b; (void)v; (void)customer; (void)pb;
    return "  *** INVOICE NOT IMPLEMENTED YET - see Billing.cpp / CONTRIBUTING.md ***\n";
}

std::string InvoiceRenderer::cancellationReceipt(const Booking& b, const Vehicle& v,
                                                 const User& customer) {
    // TODO(pricing): return a cancellation-receipt string for the given booking.
    (void)b; (void)v; (void)customer;
    return "  *** CANCELLATION RECEIPT NOT IMPLEMENTED YET - see Billing.cpp ***\n";
}

} // namespace drivesim
