# F1 Telemetry Simulator

A high-performance, multi-threaded Formula 1 telemetry data generator and processing system written in C++. This project simulates realistic F1 race telemetry with driver behavior, car characteristics, and track dynamics.

## Features

- **Real-time Telemetry Generation**: Simulates F1 race data at 50Hz (20ms intervals)
- **Beautiful Terminal Display**: Color-coded live leaderboard with emojis, progress bars, and real-time stats
- **Multi-threaded Architecture**: Producer-consumer pattern with condition variables for efficient blocking
- **2025 F1 Grid**: Full 20-driver lineup across 10 teams with realistic driver characteristics
- **Realistic Driver Profiles**: Models driver behavior including aggression, consistency, tire management, and risk tolerance
- **Car Performance Simulation**: Simulates engine power, aerodynamic efficiency, cooling, and reliability
- **Dynamic Race Positions**: Real-time position calculation based on total distance traveled
- **Tire Wear Modeling**: Progressive tire degradation based on driver aggression and track characteristics
- **Advanced Pit Stop Strategy**: Variable pit stop thresholds based on driver tire management and risk tolerance
- **Optimal Strategy Analysis (Optional)**: Find a best pit lap for selected drivers and apply it to the live race
  - Uses parallel simulation via `std::async` + `std::future` to evaluate multiple candidate pit laps
- **Driver Skill Factor**: Consistency affects how much performance drivers can extract from their cars
- **Track Configuration**: Configurable track profiles with sectors, lap length, and environmental factors

## Architecture

The system uses a producer-consumer architecture with a thread-safe ring buffer:

```
┌─────────────────┐         ┌──────────────┐         ┌─────────────────┐
│ Telemetry       │         │   Ring       │         │   Consumer      │
│ Generator       │────────▶│   Buffer     │────────▶│   Thread        │
│ (Producer)      │         │  (1024 cap)  │         │   (Display)     │
└─────────────────┘         └──────────────┘         └─────────────────┘
```

### Components

- **TelemetryGenerator**: Generates telemetry frames for all 20 drivers every 20ms, simulating speed, tire wear, sector progression, and race positions. Implements driver skill factors and variable pit stop strategies.
- **RingBuffer**: Thread-safe circular buffer using condition variables (`std::condition_variable`) for efficient blocking instead of busy-waiting. Supports graceful shutdown mechanism.
- **StrategyAnalyzer**: Optional pre-race strategy module that searches for an optimal pit lap for selected drivers.
- **RaceSimulator**: Lightweight race simulation used by the strategy analyzer to evaluate pit lap candidates.
- **Main Application**: Orchestrates strategy analysis (optional) and the producer/consumer threads, and renders the live race leaderboard.

## Building

### Requirements

- C++17 compatible compiler (GCC 7+, Clang 5+, or MSVC 2017+)
- POSIX threads support (pthread)

### Compilation

```bash
g++ -std=c++17 -I src \
  src/main.cpp \
  src/telemetry/TelemetryGenerator.cpp \
  src/strategy/RaceSimulator.cpp \
  src/strategy/StrategyAnalyzer.cpp \
  -o f1-telemetry -pthread
```

Or using clang:

```bash
clang++ -std=c++17 -I src \
  src/main.cpp \
  src/telemetry/TelemetryGenerator.cpp \
  src/strategy/RaceSimulator.cpp \
  src/strategy/StrategyAnalyzer.cpp \
  -o f1-telemetry -pthread
```

## Usage

1. **Run the simulator**:
   ```bash
   ./f1-telemetry
   ```

2. **(Optional) Run optimal strategy analysis**:
   - When prompted, type `y`
   - Enter driver indices (comma-separated, no spaces), e.g. `4,6,1`
   - The program prints the chosen pit lap per selected driver
   - Then it prints the full list of strategies that will be used and waits for **Enter** before starting the race

2. **View the live race**: The terminal displays a beautiful, color-coded leaderboard:
   ```
   🏁 LAP 15/50 🏁
   ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
   P1 🔴 Charles Leclerc      █████████░ Lap 15  Speed: 210 kph  Tire: 25%
   P2 🔵 Max Verstappen       ████████░░ Lap 14  Speed: 205 kph  Tire: 32%  
   P3 ⚪ Lewis Hamilton       ███████░░░ Lap 14  Speed: 198 kph  Tire: 28%
   ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
   ```

   **Display Features:**
   - Color-coded positions (Gold/Silver/Bronze for top 3)
   - Team emojis for all 10 teams:
     - 🔴 Ferrari, 🔵 Red Bull, ⚪ Mercedes, 🟠 McLaren
     - 🟢 Aston Martin, 💙 Alpine/Williams, ⚫ Racing Bulls/Kick Sauber
   - Progress bars showing sector completion within current lap
   - Color-coded speed (green = fast, yellow = medium, red = slow)
   - Color-coded tire wear (green = fresh, yellow = worn, red = critical)
   - Purple "[IN PITS]" indicator during pit stops
   - Real-time updates showing all 20 drivers

3. **Race end**: The simulation runs until the leader completes the configured number of laps, then prints the winner.

## Project Structure

```
f1-telemetry/
├── src/
│   ├── main.cpp                    # Main application entry point
│   ├── common/
│   │   └── types.h                 # Data structures (TelemetryFrame, DriverProfile, CarProfile, TrackProfile)
│   ├── telemetry/
│   │   ├── TelemetryGenerator.h    # Telemetry generation class interface
│   │   └── TelemetryGenerator.cpp  # Telemetry generation implementation
│   └── ingestion/
│       └── RingBuffer.h            # Thread-safe ring buffer implementation
└── README.md
```

## Data Models

### DriverProfile
Models driver behavior characteristics:
- `aggression`: Affects tire wear rate (0.0 - 1.0)
- `consistency`: Lap time variance (0.0 - 1.0)
- `tire_management`: Resistance to tire degradation (0.0 - 1.0)
- `risk_tolerance`: Willingness to take risks (0.0 - 1.0)

### CarProfile
Models car performance characteristics:
- `engine_power`: Top speed capability (0.0 - 1.0)
- `aero_efficiency`: Cornering and downforce (0.0 - 1.0)
- `cooling_efficiency`: Tire temperature stability (0.0 - 1.0)
- `reliability`: Failure probability (0.0 - 1.0)

### TelemetryFrame
Contains per-frame race data:
- Race position, timestamp, driver ID
- Lap number, sector number
- Speed, throttle, brake inputs
- Tire temperatures (FL, FR, RL, RR)
- Tire wear percentage

### TrackProfile
Defines track characteristics:
- Number of sectors, lap length
- Tire wear factor
- Overtaking difficulty
- Safety car probability

## Example Configuration

The default configuration includes the full 2025 F1 grid with 20 drivers across 10 teams:

**Top Teams:**
- **Max Verstappen** (Red Bull): High aggression (0.88), exceptional consistency (0.98), excellent tire management (0.88)
- **Charles Leclerc** (Ferrari): Very aggressive (0.94), high consistency (0.96), good tire management (0.85)
- **Lando Norris** (McLaren): High aggression (0.82), excellent consistency (0.94), good tire management (0.86)
- **Oscar Piastri** (McLaren): Moderate aggression (0.74), excellent consistency (0.90), good tire management (0.84)

**Notable Situations:**
- **Lewis Hamilton** (Ferrari): Struggling with adaptation - reduced consistency (0.72) and tire management (0.78)
- **Yuki Tsunoda** (Red Bull): Rookie at top team - low consistency (0.58) and tire management (0.58)

## Technical Details

### Thread Safety & Synchronization
- **Condition Variables**: Ring buffer uses `std::condition_variable` for efficient blocking
  - `cv_not_empty_`: Consumer waits when buffer is empty (instead of busy-waiting)
  - Eliminates CPU spinning and reduces power consumption
- **Mutex Protection**: `std::mutex` with `std::unique_lock` for thread-safe operations
- **Graceful Shutdown**: `shutdown()` method notifies all waiting threads and prevents new operations
- **Atomic Flags**: Used for race finish detection and thread coordination

### Performance
- Ring buffer capacity: 1024 frames (configurable)
- Update rate: 50Hz (20ms per frame)
- Zero busy-waiting: Condition variables ensure threads sleep when waiting
- Low-latency design: Minimal blocking between producer and consumer
- Efficient wake-up: Only one thread notified per operation (`notify_one()`)

### Advanced Simulation Features
- **Driver Skill Factor**: Speed calculation includes `driver_skill = 0.80 + consistency * 0.25`, meaning consistent drivers extract more performance
- **Variable Pit Stop Thresholds**: Range from 65-90% tire wear based on:
  - Base: `0.65 + (tire_management * 0.25)`
  - Risk adjustment: `±7.5%` based on risk tolerance
- **Variable Pit Stop Duration**: 2-3 seconds based on car reliability
- **Position Calculation**: Real-time sorting by total distance (lap distance + distance in current lap)

## Implementation Highlights

### Condition Variables
The ring buffer implementation uses condition variables to eliminate busy-waiting:

```cpp
// Consumer waits efficiently when buffer is empty
cv_not_empty_.wait(lock, [this]() { 
    return head_ != tail_ || shutdown_;
});
```

The consumer sleeps when the buffer is empty, reducing CPU usage and improving system efficiency.

### Driver Performance Model
Driver consistency directly impacts speed:
- High consistency (0.95+): Can extract 100%+ of car performance
- Low consistency (0.60): Only extracts ~80% of car performance
- This creates realistic performance gaps between drivers

### Pit Stop Strategy
Each driver has a unique pit stop threshold:
- Tire management experts (0.95): Pit at ~90% wear
- Rookies (0.58): Pit at ~70% wear (more cautious)
- Risk-takers: Pit slightly earlier, conservative drivers later

### Optimal Strategy Analysis (how it works)
When enabled at startup, the program can compute an “optimal” pit lap for a subset of drivers and feed those pit laps into the live telemetry generator.

- **Candidate laps**: `StrategyAnalyzer::PIT_LAPS_TO_TEST` defines a small set of laps to evaluate (e.g. 12, 15, 18, ...).
- **Parallel evaluation**: For a given driver, the analyzer launches multiple simulations concurrently using `std::async(std::launch::async, ...)` and collects results with `std::future<float>`.
- **Selection**: The lap with the lowest simulated finish time is chosen and applied to the live race as a single planned pit stop for that driver.

## Future Enhancements

Potential improvements:
- [ ] Weather conditions (rain, dry, mixed)
- [ ] DRS (Drag Reduction System) modeling
- [ ] Safety car deployment
- [ ] Virtual Safety Car (VSC)
- [ ] Multi-track support with track-specific characteristics
- [ ] Qualifying sessions with grid positions
- [ ] Race incidents and DNFs (crashes, mechanical failures)
- [ ] Strategy variations (one-stop vs two-stop races)
- [ ] Yellow flag periods affecting speed

