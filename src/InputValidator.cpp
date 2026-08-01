// =============================================================================
//  DriveSim  -  InputValidator.cpp
// =============================================================================
#include "InputValidator.h"

namespace drivesim {

std::string InputReader::getLine(const std::string& prompt) {
    out_ << prompt;
    out_.flush();
    std::string line;
    if (!std::getline(in_, line)) throw InputClosed();
    return line;
}

int InputReader::readIntInRange(const std::string& prompt, int lo, int hi) {
    while (true) {
        std::string s = trim(getLine(prompt));
        if (s.empty()) { out_ << "  [!] Please enter a number.\n"; continue; }
        try {
            std::size_t pos = 0;
            int value = std::stoi(s, &pos);
            if (pos != s.size()) { out_ << "  [!] '" << s << "' is not a whole number.\n"; continue; }
            if (value < lo || value > hi) {
                out_ << "  [!] Enter a value between " << lo << " and " << hi << ".\n";
                continue;
            }
            return value;
        } catch (...) {
            out_ << "  [!] '" << s << "' is not a valid number.\n";
        }
    }
}

double InputReader::readDouble(const std::string& prompt, double lo, double hi) {
    while (true) {
        std::string s = trim(getLine(prompt));
        if (s.empty()) { out_ << "  [!] Please enter a number.\n"; continue; }
        try {
            std::size_t pos = 0;
            double value = std::stod(s, &pos);
            if (pos != s.size()) { out_ << "  [!] '" << s << "' is not a number.\n"; continue; }
            if (value < lo || value > hi) {
                out_ << "  [!] Enter a value between " << lo << " and " << hi << ".\n";
                continue;
            }
            return value;
        } catch (...) {
            out_ << "  [!] '" << s << "' is not a valid number.\n";
        }
    }
}

std::string InputReader::readNonEmpty(const std::string& prompt) {
    while (true) {
        std::string s = trim(getLine(prompt));
        if (!s.empty()) return s;
        out_ << "  [!] This field cannot be blank.\n";
    }
}

std::string InputReader::readLine(const std::string& prompt) {
    return trim(getLine(prompt));
}

Date InputReader::readDate(const std::string& prompt) {
    while (true) {
        std::string s = getLine(prompt);
        Date d;
        if (Date::parse(s, d)) return d;
        out_ << "  [!] Use the format YYYY-MM-DD (e.g. 2026-08-15).\n";
    }
}

Date InputReader::readFutureDate(const std::string& prompt, const Date& notBefore) {
    while (true) {
        Date d = readDate(prompt);
        if (d >= notBefore) return d;
        out_ << "  [!] Date cannot be before " << notBefore.toString() << ".\n";
    }
}

bool InputReader::readYesNo(const std::string& prompt) {
    while (true) {
        std::string s = toLower(trim(getLine(prompt)));
        if (s == "y" || s == "yes") return true;
        if (s == "n" || s == "no")  return false;
        out_ << "  [!] Please answer y or n.\n";
    }
}

std::string InputReader::readPassword(const std::string& prompt) {
    // NOTE: a production build would disable terminal echo here (termios on
    // POSIX / SetConsoleMode on Windows). Kept portable and unmasked for the
    // assessment build.
    return readNonEmpty(prompt);
}

} // namespace drivesim
