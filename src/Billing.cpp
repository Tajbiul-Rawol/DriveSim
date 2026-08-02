// =============================================================================
//  DriveSim  -  Billing.cpp // billing features
// =============================================================================
#include "Billing.h"

#include <sstream>

namespace drivesim {

PriceBreakdown PricingEngine::quote(double rate, int days) const {
    if (days < 1) days = 1;
    PriceBreakdown pb;
    pb.days           = days;
    pb.dailyRate      = rate;
    pb.rentalSubtotal = rate * days;
    pb.serviceFee     = serviceFee_;
    pb.taxableAmount  = pb.rentalSubtotal + pb.serviceFee;
    pb.gst            = pb.taxableAmount * gstRate_;
    pb.total          = pb.taxableAmount + pb.gst;
    return pb;
}

PriceBreakdown PricingEngine::quote(const Vehicle& v, int days) const {
    return quote(v.effectiveDailyRate(), days);
}

namespace {
constexpr std::size_t W = 60;   // invoice box inner width

std::string boxTop()    { return "+" + rule(W, '-') + "+"; }
std::string boxLine(const std::string& s) { return "| " + padRight(s, W - 2) + " |"; }
// A line with a left label and right-aligned value inside the box.
std::string kv(const std::string& label, const std::string& value) {
    std::string left  = label;
    std::string right = value;
    std::size_t inner = W - 2;
    if (left.size() + right.size() > inner)
        left = left.substr(0, inner - right.size());
    std::size_t gap = inner - left.size() - right.size();
    return "| " + left + std::string(gap, ' ') + right + " |";
}
} // namespace

std::string InvoiceRenderer::invoice(const Booking& b, const Vehicle& v,
                                     const User& customer, const PriceBreakdown& pb) {
    std::ostringstream os;
    os << boxTop() << "\n";
    os << boxLine("DRIVESIM  -  TAX INVOICE") << "\n";
    os << boxLine("Fleet & Rental Management System") << "\n";
    os << "+" << rule(W, '-') << "+\n";
    os << kv("Invoice / Booking No:", b.id()) << "\n";
    os << kv("Customer:", customer.fullName() + " (" + customer.id() + ")") << "\n";
    os << boxLine("") << "\n";
    os << boxLine("Vehicle:  " + v.displayName()) << "\n";
    os << kv("Category:", v.category()) << "\n";
    os << kv("Rental period:", b.startDate().toString() + "  to  " + b.endDate().toString()) << "\n";
    os << kv("Duration:", std::to_string(pb.days) + " day(s)") << "\n";
    os << "+" << rule(W, '-') << "+\n";
    os << kv("Daily rate (incl. category):", money(pb.dailyRate)) << "\n";
    os << kv("Rental (" + std::to_string(pb.days) + " x " + money(pb.dailyRate) + "):",
             money(pb.rentalSubtotal)) << "\n";
    os << kv("Service fee:", money(pb.serviceFee)) << "\n";
    os << kv("Subtotal:", money(pb.taxableAmount)) << "\n";
    os << kv("GST (10%):", money(pb.gst)) << "\n";
    os << "+" << rule(W, '-') << "+\n";
    os << kv("TOTAL PAYABLE:", money(pb.total)) << "\n";
    os << boxTop() << "\n";
    os << "Thank you for choosing DriveSim.";
    return os.str();
}

std::string InvoiceRenderer::cancellationReceipt(const Booking& b, const Vehicle& v,
                                                 const User& customer) {
    std::ostringstream os;
    os << boxTop() << "\n";
    os << boxLine("DRIVESIM  -  CANCELLATION RECEIPT") << "\n";
    os << "+" << rule(W, '-') << "+\n";
    os << kv("Booking No:", b.id()) << "\n";
    os << kv("Customer:", customer.fullName() + " (" + customer.id() + ")") << "\n";
    os << kv("Vehicle:", v.displayName()) << "\n";
    os << kv("Original period:", b.startDate().toString() + "  to  " + b.endDate().toString()) << "\n";
    os << kv("Original total:", money(b.totalCost())) << "\n";
    os << kv("Status:", "CANCELLED") << "\n";
    os << boxTop() << "\n";
    os << "The vehicle has been released back into the available fleet.";
    return os.str();
}

} // namespace drivesim
