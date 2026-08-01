// =============================================================================
//  DriveSim  -  Common.cpp
// =============================================================================
#include "Common.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace drivesim {

// ---------------------------------------------------------------------------
//  Enum <-> string
// ---------------------------------------------------------------------------
std::string toString(VehicleStatus s) {
    switch (s) {
        case VehicleStatus::Available:   return "Available";
        case VehicleStatus::Rented:      return "Rented";
        case VehicleStatus::Maintenance: return "Maintenance";
    }
    return "Unknown";
}

std::string toString(UserRole r) {
    return r == UserRole::Admin ? "Admin" : "Customer";
}

std::string toString(BookingStatus s) {
    switch (s) {
        case BookingStatus::Confirmed: return "Confirmed";
        case BookingStatus::Cancelled: return "Cancelled";
        case BookingStatus::Completed: return "Completed";
    }
    return "Unknown";
}

bool vehicleStatusFromString(const std::string& in, VehicleStatus& out) {
    std::string s = toLower(trim(in));
    if (s == "available")   { out = VehicleStatus::Available;   return true; }
    if (s == "rented")      { out = VehicleStatus::Rented;      return true; }
    if (s == "maintenance") { out = VehicleStatus::Maintenance; return true; }
    return false;
}

bool userRoleFromString(const std::string& in, UserRole& out) {
    std::string s = toLower(trim(in));
    if (s == "admin")    { out = UserRole::Admin;    return true; }
    if (s == "customer") { out = UserRole::Customer; return true; }
    return false;
}

bool bookingStatusFromString(const std::string& in, BookingStatus& out) {
    std::string s = toLower(trim(in));
    if (s == "confirmed") { out = BookingStatus::Confirmed; return true; }
    if (s == "cancelled") { out = BookingStatus::Cancelled; return true; }
    if (s == "completed") { out = BookingStatus::Completed; return true; }
    return false;
}

// ---------------------------------------------------------------------------
//  String helpers
// ---------------------------------------------------------------------------
std::string trim(const std::string& s) {
    std::size_t a = 0, b = s.size();
    while (a < b && std::isspace(static_cast<unsigned char>(s[a]))) ++a;
    while (b > a && std::isspace(static_cast<unsigned char>(s[b - 1]))) --b;
    return s.substr(a, b - a);
}

std::string toLower(const std::string& s) {
    std::string r = s;
    std::transform(r.begin(), r.end(), r.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return r;
}

std::string toUpper(const std::string& s) {
    std::string r = s;
    std::transform(r.begin(), r.end(), r.begin(),
                   [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return r;
}

std::string simpleHash(const std::string& text) {
    std::uint64_t h = 1469598103934665603ULL;         // FNV offset basis
    for (unsigned char c : text) {
        h ^= c;
        h *= 1099511628211ULL;                          // FNV prime
    }
    std::ostringstream os;
    os << std::hex << std::setw(16) << std::setfill('0') << h;
    return os.str();
}

// ---------------------------------------------------------------------------
//  CSV helpers
// ---------------------------------------------------------------------------
std::vector<std::string> parseCsvLine(const std::string& line) {
    std::vector<std::string> out;
    std::string cur;
    bool inQuotes = false;
    for (std::size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (inQuotes) {
            if (c == '"') {
                if (i + 1 < line.size() && line[i + 1] == '"') { cur.push_back('"'); ++i; }
                else inQuotes = false;
            } else {
                cur.push_back(c);
            }
        } else {
            if (c == '"')       inQuotes = true;
            else if (c == ',')  { out.push_back(cur); cur.clear(); }
            else if (c == '\r') { /* ignore stray carriage returns */ }
            else                cur.push_back(c);
        }
    }
    out.push_back(cur);
    return out;
}

std::string csvEscape(const std::string& field) {
    bool needQuotes = field.find_first_of(",\"\n\r") != std::string::npos;
    if (!needQuotes) return field;
    std::string s = "\"";
    for (char c : field) {
        if (c == '"') s += "\"\"";
        else          s.push_back(c);
    }
    s.push_back('"');
    return s;
}

std::string makeCsvLine(const std::vector<std::string>& fields) {
    std::string s;
    for (std::size_t i = 0; i < fields.size(); ++i) {
        if (i) s.push_back(',');
        s += csvEscape(fields[i]);
    }
    return s;
}

// ---------------------------------------------------------------------------
//  Formatting helpers
// ---------------------------------------------------------------------------
std::string money(double amount) {
    bool neg = amount < 0;
    double a  = neg ? -amount : amount;
    long long cents   = static_cast<long long>(std::llround(a * 100.0));
    long long dollars = cents / 100;
    int frac          = static_cast<int>(cents % 100);

    std::string ds = std::to_string(dollars);
    std::string grouped;
    int count = 0;
    for (int i = static_cast<int>(ds.size()) - 1; i >= 0; --i) {
        grouped.push_back(ds[i]);
        if (++count % 3 == 0 && i != 0) grouped.push_back(',');
    }
    std::reverse(grouped.begin(), grouped.end());

    char buf[8];
    std::snprintf(buf, sizeof(buf), "%02d", frac);
    std::string result = "$" + grouped + "." + buf;
    return neg ? "-" + result : result;
}

std::string plainNumber(double value, int decimals) {
    std::ostringstream os;
    os << std::fixed << std::setprecision(decimals) << value;
    return os.str();
}

std::string padRight(const std::string& s, std::size_t width) {
    if (s.size() >= width) return s.substr(0, width);
    return s + std::string(width - s.size(), ' ');
}

std::string padLeft(const std::string& s, std::size_t width) {
    if (s.size() >= width) return s.substr(0, width);
    return std::string(width - s.size(), ' ') + s;
}

std::string rule(std::size_t width, char c) {
    return std::string(width, c);
}

// ---------------------------------------------------------------------------
//  Date
// ---------------------------------------------------------------------------
namespace {
// Howard Hinnant's days_from_civil: serial day count from the civil calendar.
long days_from_civil(int y, unsigned m, unsigned d) {
    y -= (m <= 2);
    const long era      = (y >= 0 ? y : y - 399) / 400;
    const unsigned yoe  = static_cast<unsigned>(y - era * 400);
    const unsigned doy  = (153 * (m > 2 ? m - 3 : m + 9) + 2) / 5 + d - 1;
    const unsigned doe  = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097L + static_cast<long>(doe) - 719468L;
}
} // namespace

bool Date::isValid(int y, int m, int d) {
    if (y < 1 || m < 1 || m > 12 || d < 1) return false;
    static const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int dim  = daysInMonth[m - 1];
    bool leap = (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    if (m == 2 && leap) dim = 29;
    return d <= dim;
}

bool Date::parse(const std::string& text, Date& out) {
    std::string t = trim(text);
    if (t.size() != 10 || t[4] != '-' || t[7] != '-') return false;
    for (int i : {0, 1, 2, 3, 5, 6, 8, 9})
        if (!std::isdigit(static_cast<unsigned char>(t[i]))) return false;
    int y = std::stoi(t.substr(0, 4));
    int m = std::stoi(t.substr(5, 2));
    int d = std::stoi(t.substr(8, 2));
    if (!isValid(y, m, d)) return false;
    out = Date(y, m, d);
    return true;
}

std::string Date::toString() const {
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%04d-%02d-%02d", y_, m_, d_);
    return buf;
}

long Date::toSerial() const {
    return days_from_civil(y_, static_cast<unsigned>(m_), static_cast<unsigned>(d_));
}

long Date::daysUntil(const Date& other) const {
    return other.toSerial() - toSerial();
}

Date Date::today() {
    std::time_t t = std::time(nullptr);
    std::tm lt{};
#if defined(_WIN32)
    localtime_s(&lt, &t);
#else
    localtime_r(&t, &lt);
#endif
    return Date(lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday);
}

} // namespace drivesim
