// =============================================================================
//  DriveSim  -  Common.h
//  Core enumerations, the Date value type, and shared string / CSV / format
//  helpers used across every module. Kept dependency-free (standard library
//  only) in line with the project's zero-dependency CLI goal.
// =============================================================================
#ifndef DRIVESIM_COMMON_H
#define DRIVESIM_COMMON_H

#include <string>
#include <vector>
#include <cstdint>

namespace drivesim {

// ---------------------------------------------------------------------------
//  Enumerations
// ---------------------------------------------------------------------------

// The three states of the vehicle status state machine.
enum class VehicleStatus { Available, Rented, Maintenance };

// Role-based access control.
enum class UserRole { Customer, Admin };

// Lifecycle of a booking.
enum class BookingStatus { Confirmed, Cancelled, Completed };

std::string toString(VehicleStatus s);
std::string toString(UserRole r);
std::string toString(BookingStatus s);

bool vehicleStatusFromString(const std::string& in, VehicleStatus& out);
bool userRoleFromString(const std::string& in, UserRole& out);
bool bookingStatusFromString(const std::string& in, BookingStatus& out);

// ---------------------------------------------------------------------------
//  String helpers
// ---------------------------------------------------------------------------

std::string trim(const std::string& s);
std::string toLower(const std::string& s);
std::string toUpper(const std::string& s);

// Non-cryptographic FNV-1a hash rendered as hex. Used only so the demo does
// not store plaintext passwords in the CSV store. NOT production security.
std::string simpleHash(const std::string& text);

// ---------------------------------------------------------------------------
//  CSV helpers  (RFC-4180-ish: supports quoted fields with escaped quotes)
// ---------------------------------------------------------------------------

std::vector<std::string> parseCsvLine(const std::string& line);
std::string csvEscape(const std::string& field);
std::string makeCsvLine(const std::vector<std::string>& fields);

// ---------------------------------------------------------------------------
//  Formatting helpers
// ---------------------------------------------------------------------------

// "$1,234.50"
std::string money(double amount);
// Plain fixed-point number for CSV storage, e.g. "1234.50"
std::string plainNumber(double value, int decimals = 2);

std::string padRight(const std::string& s, std::size_t width);
std::string padLeft(const std::string& s, std::size_t width);
std::string rule(std::size_t width, char c = '-');

// ---------------------------------------------------------------------------
//  Date  (proleptic Gregorian calendar date)
// ---------------------------------------------------------------------------
class Date {
public:
    Date() : y_(1970), m_(1), d_(1) {}
    Date(int year, int month, int day) : y_(year), m_(month), d_(day) {}

    int year()  const { return y_; }
    int month() const { return m_; }
    int day()   const { return d_; }

    // Parse "YYYY-MM-DD"; returns false (out untouched) on any malformed or
    // calendar-invalid input (e.g. 2026-02-30).
    static bool parse(const std::string& text, Date& out);

    std::string toString() const;           // "YYYY-MM-DD"

    long toSerial() const;                  // serial day number for arithmetic
    long daysUntil(const Date& other) const;// other - this (may be negative)

    bool operator<(const Date& o)  const { return toSerial() <  o.toSerial(); }
    bool operator<=(const Date& o) const { return toSerial() <= o.toSerial(); }
    bool operator>(const Date& o)  const { return toSerial() >  o.toSerial(); }
    bool operator>=(const Date& o) const { return toSerial() >= o.toSerial(); }
    bool operator==(const Date& o) const { return toSerial() == o.toSerial(); }

    static bool isValid(int year, int month, int day);
    static Date today();

private:
    int y_, m_, d_;
};

} // namespace drivesim

#endif // DRIVESIM_COMMON_H
