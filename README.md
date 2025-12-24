# F1 Telemetry Simulator

A high-performance, multi-threaded Formula 1 telemetry data generator and processing system written in C++. This project simulates realistic F1 race telemetry with driver behavior, car characteristics, and track dynamics.

## Features

- **Real-time Telemetry Generation**: Simulates F1 race data at 50Hz (20ms intervals)
- **Multi-threaded Architecture**: Producer-consumer pattern with lock-free ring buffer for efficient data flow
- **Realistic Driver Profiles**: Models driver behavior including aggression, consistency, tire management, and risk tolerance
- **Car Performance Simulation**: Simulates engine power, aerodynamic efficiency, cooling, and reliability
- **Dynamic Race Positions**: Real-time position calculation based on total distance traveled
- **Tire Wear Modeling**: Progressive tire degradation based on driver aggression and track characteristics
- **Track Configuration**: Configurable track profiles with sectors, lap length, and environmental factors

## Architecture

The system uses a producer-consumer architecture with a thread-safe ring buffer:

```
┌─────────────────┐         ┌──────────────┐         ┌─────────────────┐
│ Telemetry       │         │   Ring       │         │   Consumer      │
│ Generator       │────────▶│   Buffer     │────────▶│   Thread        │
│ (Producer)      │         │  (1024 cap) │         │   (Display)     │
└─────────────────┘         └──────────────┘         └─────────────────┘
```

### Components

- **TelemetryGenerator**: Generates telemetry frames for all drivers every 20ms, simulating speed, tire wear, sector progression, and race positions
- **RingBuffer**: Thread-safe circular buffer for efficient data transfer between producer and consumer threads
- **Main Application**: Orchestrates the producer and consumer threads, handles user input for graceful shutdown

## Building

### Requirements

- C++17 compatible compiler (GCC 7+, Clang 5+, or MSVC 2017+)
- POSIX threads support (pthread)

### Compilation

```bash
g++ -std=c++17 -I src src/main.cpp src/telemetry/TelemetryGenerator.cpp -o f1-telemetry -pthread
```

Or using clang:

```bash
clang++ -std=c++17 -I src src/main.cpp src/telemetry/TelemetryGenerator.cpp -o f1-telemetry -pthread
```

## Usage

1. **Run the simulator**:
   ```bash
   ./f1-telemetry
   ```

2. **View telemetry output**: The consumer thread will display real-time telemetry data:
   ```
   [Telemetry] P1 Max Verstappen Lap 2 Sector 1 Speed 209.5
   [Telemetry] P2 Lewis Hamilton Lap 2 Sector 1 Speed 198.3
   [Telemetry] P3 Charles Leclerc Lap 2 Sector 1 Speed 215.2
   ```

3. **Stop the simulation**: Press Enter to gracefully shutdown all threads

## Project Structure

```
f1-telemetry/
├── src/
│   ├── main.cpp                    # Main application entry point
│   ├── common/
│   │   └── types.h                 # Data structures (TelemetryFrame, DriverProfile, CarProfile, TrackProfile)
│   ├── telemetry/
│   │   ├── TelemetryGenerator.h   # Telemetry generation class interface
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

The default configuration includes three drivers with realistic profiles:

- **Max Verstappen** (Red Bull): High aggression (0.85), excellent consistency (0.90), good tire management (0.80)
- **Lewis Hamilton** (Mercedes): Balanced aggression (0.65), exceptional consistency (0.95), excellent tire management (0.95)
- **Charles Leclerc** (Ferrari): Very aggressive (0.90), moderate consistency (0.75), moderate tire management (0.70)

## Technical Details

### Thread Safety
- Ring buffer uses mutex-based synchronization for thread-safe push/pop operations
- Atomic boolean flag for graceful shutdown coordination
- Producer thread generates data at fixed 20ms intervals
- Consumer thread processes data with minimal latency

### Performance
- Ring buffer capacity: 1024 frames (configurable)
- Update rate: 50Hz (20ms per frame)
- Backpressure handling: Drops oldest frame when buffer is full
- Low-latency design: Minimal blocking between producer and consumer

## Future Enhancements

Potential improvements:
- [ ] Network export (UDP/TCP telemetry streaming)
- [ ] File logging (CSV/JSON output)
- [ ] Pit stop simulation
- [ ] Weather conditions
- [ ] DRS (Drag Reduction System) modeling
- [ ] Safety car deployment
- [ ] Multi-track support
- [ ] Real-time visualization

## License

This project is provided as-is for educational and demonstration purposes.

