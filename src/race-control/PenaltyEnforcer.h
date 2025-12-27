#pragma once

#include "../common/types.h"
#include <map>
#include <chrono>
#include <mutex>

enum class PenaltyState {
    NONE,
    SERVING,
    SERVED,
    PENDING
};

struct DriverPenaltyInfo {
    PenaltyState state;
    uint32_t penalty_seconds;
    std::chrono::steady_clock::time_point penalty_start;

};

class PenaltyEnforcer {
public:
    PenaltyEnforcer(const std::vector<DriverProfile>& drivers);

    void issuePenalty(uint32_t driver_id, uint32_t seconds);
    bool shouldServePenalty(uint32_t driver_id);
    bool isPenaltyComplete(uint32_t driver_id);
    DriverPenaltyInfo getPenaltyInfo(uint32_t driver_id);
    
private:
    std::map<uint32_t, DriverPenaltyInfo> penalties_;
    mutable std::mutex mutex_;
};
