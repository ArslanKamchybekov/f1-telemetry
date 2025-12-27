#include "TrackLimitsMonitor.h"
#include <random>
#include <mutex>

using namespace std;

static std::random_device rd;
static std::mt19937 gen(rd());
std::uniform_real_distribution<float> dis(0.0f, 1.0f);

TrackLimitsMonitor::TrackLimitsMonitor(
    const TrackProfile &track,
    const vector<DriverProfile> &drivers
) : track_(track), drivers_(drivers) {
    for(uint32_t i = 0; i < drivers.size(); i++) {
        driver_violations_.insert({i, TrackLimitsState{0, false, {}}});
    }
}

void TrackLimitsMonitor::processFrame(const TelemetryFrame &frame) {
    // Only check for violations at sector boundaries (not every frame)
    static std::map<uint32_t, uint8_t> last_sector;
    
    if (last_sector[frame.driver_id] != frame.sector) {
        last_sector[frame.driver_id] = frame.sector;
        
        const auto &driver = drivers_[frame.driver_id];
        float aggression_factor = driver.aggression * 0.01f;
        float speed_factor = (frame.speed_kph > 200.0f) ? 0.005f : 0.0f;
        float tire_wear_factor = (frame.tire_wear > 0.6f) ? frame.tire_wear * 0.01f : 0.0f;
        float violation_probability = aggression_factor + speed_factor + tire_wear_factor;

        if(dis(gen) < violation_probability) {
            unique_lock<mutex> lock(mutex_);
            auto &state = driver_violations_[frame.driver_id];
            state.warnings += 1;
            state.violation_laps.push_back(frame.lap);

            if(state.warnings >= 3) {
                state.has_penalty = true;
            }

            lock.unlock();
        }
    }
}

TrackLimitsState TrackLimitsMonitor::getDriverState(uint32_t driver_id) const {
    lock_guard<mutex> lock(mutex_);
    return driver_violations_.at(driver_id);
}
