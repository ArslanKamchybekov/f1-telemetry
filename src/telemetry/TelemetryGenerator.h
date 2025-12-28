#pragma once

#include <vector>
#include <map>
#include <cstdint>
#include "../common/types.h"
#include "../race-control/PenaltyEnforcer.h"

class TelemetryGenerator {
public:
    TelemetryGenerator(const TrackProfile& track, const std::vector<DriverProfile>& drivers, const std::vector<CarProfile>& cars, uint32_t total_laps, std::shared_ptr<PenaltyEnforcer> penalty_enforcer);

    std::vector<TelemetryFrame> next();
    bool isRaceFinished() const;

    void setOptimalStrategies(const std::map<uint32_t, uint32_t>& strategies);

private:
    TrackProfile track_;
    std::vector<DriverProfile> drivers_;
    std::vector<CarProfile> cars_;
    uint32_t total_laps_;

    uint64_t current_time_ns_; // simulation time

    std::map<uint32_t, uint32_t> optimal_strategies_;

    std::vector<DriverState> states_;

    std::shared_ptr<PenaltyEnforcer> penalty_enforcer_;

    TelemetryFrame generateFrame(uint32_t driver_id);

    void calculatePositions(std::vector<TelemetryFrame>& frames);

    float getTotalDistance(uint32_t driver_id) const;
};