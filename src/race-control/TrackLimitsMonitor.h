#pragma once

#include "../common/types.h"
#include <vector>
#include <map>
#include <mutex>
#include "PenaltyEnforcer.h"

struct TrackLimitsState {
    uint32_t warnings;
    bool has_penalty;
    std::vector<uint32_t> violation_laps;
};

class TrackLimitsMonitor{
public:
    TrackLimitsMonitor(const TrackProfile& track, const std::vector<DriverProfile>& drivers, std::shared_ptr<PenaltyEnforcer> penalty_enforcer);

    void processFrame(const TelemetryFrame& frame);

    TrackLimitsState getDriverState(uint32_t driver_id) const;

private:
    TrackProfile track_;
    std::vector<DriverProfile> drivers_;
    std::shared_ptr<PenaltyEnforcer> penalty_enforcer_;

    std::map<uint32_t, TrackLimitsState> driver_violations_;
    mutable std::mutex mutex_;
};