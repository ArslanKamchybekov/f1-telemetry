#pragma once

#include <cstdint>
#include <string>

struct CarProfile {
    std::string car_id;

    // Performance characteristics (0.0 – 1.0)
    float engine_power;        // top speed
    float aero_efficiency;     // cornering speed, tire load
    float cooling_efficiency;  // tire temperature stability
    float reliability;         // failure probability (optional later)
};

struct DriverProfile {
    std::string driver_id;

    // behavioral traits (linearized to [0, 1])
    float aggression; // tire wear
    float consistency; // lap time variance
    float tire_management; // degradation resistance
    float risk_tolerance; // willingness to pit under uncertainty
};

struct TelemetryFrame {
    uint8_t race_position;

    uint64_t timestamp_ns;

    uint32_t driver_id;
    uint32_t lap;
    uint8_t  sector;

    // Vehicle state
    float speed_kph;
    float throttle;            // 0.0 – 1.0
    float brake;               // 0.0 – 1.0

    // Tires
    float tire_temp_c[4];      // FL, FR, RL, RR
    float tire_wear;           // 0.0 (new) – 1.0 (dead)
};

struct TrackProfile {
    uint32_t track_id;

    uint8_t  sectors;               // usually 3
    float lap_length_km;

    // Environmental multipliers
    float tire_wear_factor;          // baseline degradation
    float overtaking_difficulty;     // affects traffic loss
    float safety_car_probability;    // per lap
};

struct DriverState {
    uint32_t lap;
    uint8_t sector;
    float tire_wear;
    float distance_in_lap;

    bool is_on_pit;
    bool has_pitted;  // Prevent re-triggering a planned/optimal pit stop
    uint64_t pit_stop_start_time_ns;
    uint64_t pit_stop_end_time_ns;
};