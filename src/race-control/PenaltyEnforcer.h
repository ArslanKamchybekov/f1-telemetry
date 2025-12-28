#pragma once

#include "../common/types.h"
#include <map>
#include <cstdint>
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
    uint64_t penalty_start_time_ns;
    uint64_t penalty_duration_ns;

};

class PenaltyEnforcer {
public:
    PenaltyEnforcer(const std::vector<DriverProfile>& drivers);

    void issuePenalty(uint32_t driver_id, uint32_t seconds);
    bool shouldServePenalty(uint32_t driver_id, uint64_t current_time_ns);
    bool isPenaltyComplete(uint32_t driver_id, uint64_t current_time_ns);
    DriverPenaltyInfo getPenaltyInfo(uint32_t driver_id) const;
    
private:
    std::map<uint32_t, DriverPenaltyInfo> penalties_;
    mutable std::mutex mutex_;
};
