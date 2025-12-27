#include "PenaltyEnforcer.h"

using namespace std;

PenaltyEnforcer::PenaltyEnforcer(const std::vector<DriverProfile>& drivers) {
    for(uint32_t i = 0; i < drivers.size(); i++) {
        penalties_.insert({i, {PenaltyState::NONE, 0, chrono::steady_clock::now()}});
    }
}

void PenaltyEnforcer::issuePenalty(uint32_t driver_id, uint32_t seconds) {
    lock_guard<mutex> lock(mutex_);
    
    auto &info = penalties_[driver_id];
    info.state = PenaltyState::PENDING;
    info.penalty_seconds = seconds;
}

bool PenaltyEnforcer::shouldServePenalty(uint32_t driver_id) {
    lock_guard<mutex> lock(mutex_);
    auto &info = penalties_[driver_id];
    if(info.state == PenaltyState::PENDING) {
        info.state = PenaltyState::SERVING;
        info.penalty_start = chrono::steady_clock::now();
        return true;
    }
    return false;
}

bool PenaltyEnforcer::isPenaltyComplete(uint32_t driver_id) {
    lock_guard<mutex> lock(mutex_);

    auto &info = penalties_[driver_id];
    if(info.state != PenaltyState::SERVING) {
        return true;
    }

    auto now = chrono::steady_clock::now();
    auto seconds_elapsed = chrono::duration_cast<chrono::seconds>(now - info.penalty_start).count();

    if(seconds_elapsed >= info.penalty_seconds) {
        info.state = PenaltyState::SERVED;
        return true;
    }
    return false;
}

DriverPenaltyInfo PenaltyEnforcer::getPenaltyInfo(uint32_t driver_id) {
    lock_guard<mutex> lock(mutex_);
    return penalties_[driver_id];
}