// =============================================================================
//  DriveSim  -  InputValidator.h
//  Defensive console input. Every reader validates type / range / semantics
//  and re-prompts with a descriptive message on bad input, so the rest of the
//  application only ever receives well-formed values. A closed input stream
//  (EOF) raises InputClosed, letting the application exit cleanly instead of
//  spinning in a re-prompt loop.
// =============================================================================
#ifndef DRIVESIM_INPUTVALIDATOR_H
#define DRIVESIM_INPUTVALIDATOR_H

#include <iostream>
#include <stdexcept>
#include <string>

#include "Common.h"

namespace drivesim {

// Thrown when the input stream is exhausted (e.g. Ctrl-D / piped EOF).
struct InputClosed : std::runtime_error {
    InputClosed() : std::runtime_error("input stream closed") {}
};

class InputReader {
public:
    explicit InputReader(std::istream& in = std::cin, std::ostream& out = std::cout)
        : in_(in), out_(out) {}

    int         readIntInRange(const std::string& prompt, int lo, int hi);
    double      readDouble(const std::string& prompt, double lo, double hi);
    std::string readNonEmpty(const std::string& prompt);
    std::string readLine(const std::string& prompt);     // may be empty
    Date        readDate(const std::string& prompt);      // reject past dates?
    Date        readFutureDate(const std::string& prompt, const Date& notBefore);
    bool        readYesNo(const std::string& prompt);
    std::string readPassword(const std::string& prompt);  // portable, unmasked

private:
    // Reads one whole line; throws InputClosed at end of stream.
    std::string getLine(const std::string& prompt);

    std::istream& in_;
    std::ostream& out_;
};

} // namespace drivesim

#endif // DRIVESIM_INPUTVALIDATOR_H
