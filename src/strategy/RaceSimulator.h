#pragma once

#include "../common/types.h"
#include <vector>
#include <cstdint>
#include <map>

class RaceSimulator {
public:
    RaceSimulator(
        const TrackProfile& track, 
        const std::vector<DriverProfile>& drivers, 
        const std::vector<CarProfile>& cars, 
        uint32_t total_laps
    );

    float simulateRace(uint32_t driver_id, uint32_t pit_lap);

private:
    struct DriverSimState {
        uint32_t lap;
        uint8_t sector;
        float tire_wear;
        float distance_in_lap;
        float total_time_seconds;
        bool has_pitted;
        bool is_on_pit;
    };

    TrackProfile track_;
    std::vector<DriverProfile> drivers_;
    std::vector<CarProfile> cars_;
    uint32_t total_laps_;

    std::vector<DriverSimState> states_;

    void simulateTick();
    void updateDriverState(uint32_t driver_id, uint32_t target_driver_id, uint32_t forced_pit_lap);
    bool shouldPit(uint32_t driver_id, uint32_t target_driver_id, uint32_t forced_pit_lap);
};
