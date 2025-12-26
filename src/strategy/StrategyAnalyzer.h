#pragma once

#include "../common/types.h"
#include "RaceSimulator.h"
#include <vector>
#include <cstdint>
#include <string>

struct StrategyResult {
    uint32_t driver_id;
    uint32_t optimal_pit_lap; 
    float finish_time_seconds;
};

class StrategyAnalyzer {
public:
    StrategyAnalyzer(
        const TrackProfile& track,
        const std::vector<DriverProfile>& drivers,
        const std::vector<CarProfile>& cars,
        uint32_t total_laps
    );

    std::vector<StrategyResult> analyzeStrategies(const std::vector<uint32_t>& driver_ids_to_optimize);

private:
    TrackProfile track_;
    std::vector<DriverProfile> drivers_;
    std::vector<CarProfile> cars_;
    uint32_t total_laps_;

    static const std::vector<uint32_t> PIT_LAPS_TO_TEST;

    StrategyResult findOptimalForDriver(uint32_t driver_id);
};
