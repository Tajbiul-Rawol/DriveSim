# Contributing to DriveSim

This repository is the **skeleton** for the DriveSim Fleet & Rental Management
System. The architecture, core plumbing and several features are already
implemented and the project **compiles and runs as-is**. Two feature modules are
deliberately left as stubs for the team to implement.

Please read this whole file before you start.

---

## What is already done

Fully implemented and working:

- **Common** — `Date`, CSV parsing, formatting helpers, enums
- **Vehicle** (+ Sedan/SUV/Electric/LuxuryVan) — domain model and the
  Available/Rented/Maintenance **state machine**
- **User** (+ Customer/Admin) — role-based accounts and authentication
- **Booking** — the booking record
- **DataStore** — the in-memory repository (owns all data, provides queries)
- **Persistence** — CSV load/save and first-run seeding
- **InputValidator** — defensive console input
- **Application** — the full menu UI for both roles (this file is complete;
  **do not edit it** — it already calls into the modules below)

## What YOU implement (the two open modules)

| Module | SRS feature | File(s) to edit | Suggested owner |
|--------|-------------|-----------------|-----------------|
| **Billing** | Feature 4 — Dynamic Pricing & Invoice Generation | `src/Billing.cpp` | Teammate A |
| **Analytics** | Feature 5 — Administrative Analytics | `src/Analytics.cpp` | Teammate B |

Each `.cpp` currently returns placeholder text so the app runs. The matching
`.h` header is the **contract** — implement the bodies, do not change the
signatures.

---

## Build & run

```bash
# From the project root
make            # or: g++ -std=c++17 -O2 src/*.cpp -o drivesim
./drivesim
```

Visual Studio users: **File → Open → Folder…** and select this folder (it has a
`CMakeLists.txt`), pick `drivesim.exe` as the startup item, then Ctrl+F5.

Default logins for testing: `admin` / `admin123` and `demo` / `demo123`.

Delete the `data/` CSV files any time to reset to seeded defaults.

---

## Brief — Teammate A: Billing (`src/Billing.cpp`)

Implement four methods (signatures already in `Billing.h`):

1. `PricingEngine::quote(const Vehicle&, int days)` — return a fully populated
   `PriceBreakdown`:
   ```
   dailyRate      = vehicle.effectiveDailyRate()   // base rate x category multiplier
   rentalSubtotal = dailyRate * days
   taxableAmount  = rentalSubtotal + serviceFee_   // serviceFee_ defaults to 25.00
   gst            = taxableAmount * gstRate_        // gstRate_ defaults to 0.10 (10%)
   total          = taxableAmount + gst
   ```
2. `PricingEngine::quote(double effectiveDailyRate, int days)` — same maths from
   a raw rate.
3. `InvoiceRenderer::invoice(...)` — return a formatted multi-line tax invoice
   (invoice/booking no, customer, vehicle, rental period, duration, itemised
   charges, GST, total). Use `money()`, `padRight()`, `padLeft()`, `rule()` from
   `Common.h`.
4. `InvoiceRenderer::cancellationReceipt(...)` — a short cancellation receipt.

**Acceptance criteria**
- Creating a booking as `demo` shows a correct total. Example: a 4-day booking on
  the Tesla Model 3 (effective rate $108.00/day) → rental $432.00 + $25.00 fee =
  $457.00, GST $45.70, **total $502.70**.
- The invoice is neatly aligned and shows every line item.
- Cancelling a booking prints a cancellation receipt.
- Builds clean under `-Wall -Wextra`.

## Brief — Teammate B: Analytics (`src/Analytics.cpp`)

Implement four report methods (signatures in `Analytics.h`); `dashboard()` is
already wired to call them. Data comes from `store_` (the `DataStore`):

1. `fleetUtilization()` — count vehicles by status
   (Available/Rented/Maintenance) from `store_.vehicles()`; show counts, shares,
   and overall utilisation (rented / fleet).
2. `revenueByCategory()` — for bookings that are **Confirmed or Completed**, group
   `totalCost()` by the vehicle's category (look each vehicle up with
   `store_.findVehicle(booking.vehicleId())`); show bookings, revenue and share
   per category.
3. `bookingFrequency()` — count bookings by status.
4. `topRevenueVehicles(int topN)` — rank vehicles by revenue (Confirmed/Completed
   bookings), highest first; list the top N.

**Acceptance criteria**
- With no bookings, reports render headers and a sensible "no data" line (no
  crashes, no divide-by-zero).
- After the example booking above, revenue-by-category attributes $502.70 to
  Electric and the top-vehicle report lists the Tesla with $502.70.
- Tables are aligned using the `Common.h` helpers.
- Builds clean under `-Wall -Wextra`.

---

## Ground rules

- **Never change a signature in a `.h` file.** Those are the shared contracts.
- **Don't edit `Application.cpp` or any module you weren't assigned.** If you
  think you need to, raise it in the PR instead.
- Keep the zero-dependency, standard-library-only constraint (no new libraries).
- The build must be warning-free (`-Wall -Wextra -Wpedantic`) before you open a
  pull request.
- Test the default logins and the flows your feature touches before pushing.

## Git workflow

```bash
# One-time
git clone <repo-url>
cd DriveSim

# Start your feature on a branch (never commit straight to main)
git checkout -b feat/billing          # Teammate A
# or
git checkout -b feat/analytics        # Teammate B

# ... implement, build, test ...

git add src/Billing.cpp               # only the file(s) you own
git commit -m "Implement pricing engine and invoice rendering (SRS F4)"
git push -u origin feat/billing
```

Then open a **Pull Request** into `main` on GitHub and request a review. Because
each of you edits a different file, your PRs won't conflict.
