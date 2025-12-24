#pragma once

#include <vector>
#include <cstdint>
#include "../common/types.h"

using namespace std;

class TelemetryGenerator {
public:
    TelemetryGenerator(const TrackProfile& track, const vector<DriverProfile>& drivers, const vector<CarProfile>& cars, uint32_t total_laps);

    vector<TelemetryFrame> next();
    bool isRaceFinished() const;

private:
    TrackProfile track_;
    vector<DriverProfile> drivers_;
    vector<CarProfile> cars_;
    uint32_t total_laps_;

    uint64_t current_time_ns_; // simulation time

    vector<DriverState> states_;

    TelemetryFrame generateFrame(uint32_t driver_id);

    void calculatePositions(vector<TelemetryFrame>& frames);

    float getTotalDistance(uint32_t driver_id) const;
};