#include "PenaltyEnforcer.h"

using namespace std;

PenaltyEnforcer::PenaltyEnforcer(const std::vector<DriverProfile>& drivers) {
    for(uint32_t i = 0; i < drivers.size(); i++) {
        penalties_.insert({i, {PenaltyState::NONE, 0, 0ULL, 0ULL}});
    }
}

void PenaltyEnforcer::issuePenalty(uint32_t driver_id, uint32_t seconds) {
    lock_guard<mutex> lock(mutex_);
    
    auto &info = penalties_[driver_id];
    info.state = PenaltyState::PENDING;
    info.penalty_seconds = seconds;
    info.penalty_duration_ns = static_cast<uint64_t>(seconds) * 1'000'000'000ULL;
    info.penalty_start_time_ns = 0ULL;
}

bool PenaltyEnforcer::shouldServePenalty(uint32_t driver_id, uint64_t current_time_ns) {
    lock_guard<mutex> lock(mutex_);
    auto &info = penalties_[driver_id];
    if(info.state == PenaltyState::PENDING) {
        info.state = PenaltyState::SERVING;
        info.penalty_start_time_ns = current_time_ns;
        return true;
    }
    return false;
}

bool PenaltyEnforcer::isPenaltyComplete(uint32_t driver_id, uint64_t current_time_ns) {
    lock_guard<mutex> lock(mutex_);

    auto &info = penalties_[driver_id];
    if(info.state == PenaltyState::NONE) {
        return true;
    }
    if(info.state == PenaltyState::SERVED) {
        return true;
    }
    if(info.state == PenaltyState::PENDING) {
        return false;
    }
    // SERVING: complete once simulated time has elapsed.
    if(current_time_ns >= info.penalty_start_time_ns + info.penalty_duration_ns) {
        info.state = PenaltyState::SERVED;
        return true;
    }
    return false;
}

DriverPenaltyInfo PenaltyEnforcer::getPenaltyInfo(uint32_t driver_id) const {
    lock_guard<mutex> lock(mutex_);
    return penalties_.at(driver_id);
}