# DriveSim — Terminal Fleet & Rental Management System

A dependency-free C++17 command-line application for small vehicle-rental
operators. It manages a vehicle fleet, customer accounts, bookings, invoicing
and administrative analytics, with all state persisted to plain CSV files (no
database, no external libraries).

This is the implementation of the DriveSim project proposed in the SEP401
Assessment 1 SRS.

## Project status (team skeleton)

The architecture, core plumbing and several features are complete, and the
project **compiles and runs as-is**. Two feature modules are left as stubs for
the team to implement:

- **Billing** (`src/Billing.cpp`) — Dynamic Pricing & Invoice Generation
- **Analytics** (`src/Analytics.cpp`) — Administrative Analytics

Until they are implemented these show placeholder text at runtime (booking
totals read `$0.00` and the analytics reports print a "not implemented" line).
See **[CONTRIBUTING.md](CONTRIBUTING.md)** for the per-module brief, acceptance
criteria and Git workflow.

---

## Quick start

```bash
# Build (needs g++ 9+ or any C++17 compiler)
make

# Run
./drivesim
```

On first launch the program seeds a default admin account, a demo customer and
a starter fleet, then writes them to `data/`.

| Role     | Username | Password   |
|----------|----------|------------|
| Admin    | `admin`  | `admin123` |
| Customer | `demo`   | `demo123`  |

You can also register your own customer account from the main menu.

### Other build options

```bash
# CMake
cmake -S . -B build && cmake --build build

# Or compile directly
g++ -std=c++17 -O2 src/*.cpp -o drivesim
```

**Windows (MSVC):** open the folder in Visual Studio (CMake project) or run
`cl /std:c++17 /EHsc src\*.cpp /Fe:drivesim.exe` from a Developer Prompt.

---

## Features (mapped to the SRS)

| SRS core feature | Where it lives |
|---|---|
| **Vehicle fleet management** — abstract `Vehicle` base with `Sedan`, `SUV`, `ElectricVehicle`, `LuxuryVan` derived types; polymorphic pricing | `Vehicle.h/.cpp` |
| **Customer registration & authentication** — role-based `Customer`/`Admin`, hashed passwords, persisted profiles | `User.h/.cpp`, `Application::doLogin/doRegister` |
| **Booking creation & management** — browse by category / rate / availability, create, view, cancel | `Booking.h/.cpp`, `Application` customer flows |
| **Dynamic pricing & invoice generation** — effective daily rate × duration + service fee + 10% GST, formatted tax invoice | `Billing.h/.cpp` |
| **Administrative analytics** — utilisation, revenue by category, booking frequency, top-revenue vehicles as ASCII tables | `Analytics.h/.cpp` |
| **Defensive input validation** — type/range/semantic checks with re-prompting | `InputValidator.h/.cpp` |
| **Persistent records** — CSV load/save with default seeding | `Persistence.h/.cpp` |

---

## Architecture

The code is split into module boundaries matching the SRS's three-developer
plan (persistence / domain / pricing-UI), each with a single responsibility:

```
src/
  Common.*          Enums, Date value type, string/CSV/format helpers
  Vehicle.*         Abstract Vehicle + concrete categories + factory (domain)
  User.*            User/Customer/Admin, role-based access (domain)
  Booking.*         Booking entity (domain)
  Billing.*         PricingEngine + InvoiceRenderer (pricing)
  DataStore.*       In-memory repository, queries, ID generation
  Persistence.*     CSV load/save + seeding (persistence)
  Analytics.*       Admin reporting (analytics)
  InputValidator.*  Defensive console input (UI support)
  Application.*     Menu controller for both roles (presentation)
  main.cpp          Entry point
```

Dependencies flow one way: the domain and service modules never include the UI.
`DataStore` owns all domain objects (`std::unique_ptr<Vehicle>`,
`std::unique_ptr<User>`, `Booking` by value); persistence and analytics operate
against that single store.

### Design notes

- **Vehicle status state machine.** Status is one of `Available`, `Rented`,
  `Maintenance`. Transitions are validated in one place
  (`Vehicle::isTransitionAllowed`): `Available → Rented`,
  `Available ↔ Maintenance`, `Rented → Available`. A vehicle that is rented or
  under maintenance cannot be booked, and a rented vehicle must be returned
  before it can be serviced or removed. This is how "scheduling conflicts are
  validated before confirmation" from the SRS is enforced.

- **Open/Closed Principle.** `categoryMultiplier()` and `features()` are
  polymorphic on `Vehicle`. Adding a new category (e.g. a `Convertible`) means
  adding one subclass and one line in the factory — the pricing engine,
  analytics and UI need no changes.

- **Pricing.** `total = (effectiveRate × days) + serviceFee`, then 10% GST on
  that subtotal, where `effectiveRate = baseRate × categoryMultiplier`. Default
  multipliers: Sedan 1.00, Electric 1.20, SUV 1.35, LuxuryVan 1.80. The service
  fee ($25) and GST rate are constructor parameters on `PricingEngine`.

- **Passwords** are stored as a non-cryptographic FNV-1a hash purely so the demo
  CSV holds no plaintext. A production build would use a real KDF (bcrypt /
  Argon2) and mask terminal input.

---

## Data files

Written to `data/` next to the executable:

- `vehicles.csv` — `id,category,make,model,year,baseDailyRate,status`
- `users.csv` — `role,id,username,passwordHash,fullName,email,phone`
- `bookings.csv` — `id,customerId,vehicleId,start,end,totalCost,status`

Header lines begin with `#` and are ignored on load. Deleting the files (or the
whole `data/` folder) resets the system to its seeded defaults on next run.

---

## Scope / simplifications

The booking model treats a booking as the vehicle's current active rental (the
state machine guarantees one active rental per vehicle at a time). A full
calendar-reservation model supporting multiple non-overlapping future bookings
per vehicle is a natural extension of `Booking` + `DataStore` and does not
affect the module boundaries above.
"# DriveSim" 
