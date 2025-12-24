
#include "TelemetryGenerator.h"

using namespace std;

TelemetryGenerator::TelemetryGenerator(
    const TrackProfile& track,
    const vector<DriverProfile>& drivers,
    const vector<CarProfile>& cars,
    uint32_t total_laps
) : track_(track), drivers_(drivers), cars_(cars), total_laps_(total_laps), current_time_ns_(0) {
    states_.resize(drivers.size()); // initialize driver states

    for (auto &s : states_){
        s.lap = 0;
        s.sector = 0;
        s.tire_wear = 0.0f;
        s.distance_in_lap = 0.0f;
    }
}

// Generate a single frame for a driver every 20ms
vector<TelemetryFrame> TelemetryGenerator::next() {
    constexpr uint64_t tick_ns = 20 * 1e6;
    current_time_ns_ += tick_ns;

    vector<TelemetryFrame> frames;
    frames.reserve(drivers_.size());

    for(uint32_t i = 0; i < drivers_.size(); i++) {
        frames.push_back(generateFrame(i));
    }

    calculatePositions(frames);

    return frames;
}

float TelemetryGenerator::getTotalDistance(uint32_t driver_id) const {
    return states_[driver_id].distance_in_lap + states_[driver_id].lap * track_.lap_length_km;
}

void TelemetryGenerator::calculatePositions(vector<TelemetryFrame>& frames) {
    vector<pair<uint32_t, float>> positions;
    for(uint32_t i = 0; i < drivers_.size(); i++) {
        positions.push_back({i, getTotalDistance(i)});
    }
    sort(positions.begin(), positions.end(), [](const pair<uint32_t, float>& a, const pair<uint32_t, float>& b) {
        return a.second > b.second;
    });
    for(uint32_t i = 0; i < positions.size(); i++) {
        uint32_t driver_id = positions[i].first;
        frames[driver_id].race_position = i + 1;
    }
}

// Generate a single frame for a driver
TelemetryFrame TelemetryGenerator::generateFrame(uint32_t i) {
    auto& state = states_[i];
    const auto& driver = drivers_[i];
    const auto& car = cars_[i];

    // base speed model
    float speed =
        220.0f
        * car.engine_power
        * (1.0f - state.tire_wear * 0.4f);

    // tire wear update
    state.tire_wear +=
        0.0005f
        * track_.tire_wear_factor
        * driver.aggression;

    if (state.tire_wear > 1.0f)
        state.tire_wear = 1.0f;

    // distance update
    state.distance_in_lap += speed * 0.005f; // scaled

    // sector progression
    float sector_length = track_.lap_length_km / track_.sectors;
    if (state.distance_in_lap >= sector_length) {
        state.distance_in_lap = state.distance_in_lap - sector_length;
        state.sector++;

        if (state.sector > track_.sectors) {
            state.sector = 1;
            state.lap++;
        }
    }

    TelemetryFrame frame{};
    frame.timestamp_ns = current_time_ns_;
    frame.driver_id = i;
    frame.lap = state.lap;
    frame.sector = state.sector;
    frame.speed_kph = speed;
    frame.throttle = 1.0f;
    frame.brake = 0.0f;
    frame.tire_wear = state.tire_wear;

    return frame;
}
