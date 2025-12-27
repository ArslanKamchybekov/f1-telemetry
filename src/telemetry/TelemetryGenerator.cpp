
#include "TelemetryGenerator.h"
#include <algorithm>

using namespace std;

TelemetryGenerator::TelemetryGenerator(
    const TrackProfile& track,
    const vector<DriverProfile>& drivers,
    const vector<CarProfile>& cars,
    uint32_t total_laps
) : track_(track), drivers_(drivers), cars_(cars), total_laps_(total_laps), current_time_ns_(0) {
    states_.resize(drivers.size());

    for (auto &s : states_){
        s.lap = 0;
        s.sector = 1;
        s.tire_wear = 0.0f;
        s.distance_in_lap = 0.0f;
        s.is_on_pit = false;
        s.has_pitted = false;
        s.pit_stop_start_time_ns = 0;
        s.pit_stop_end_time_ns = 0;
    }
}

vector<TelemetryFrame> TelemetryGenerator::next() {
    constexpr uint64_t tick_ns = 20'000'000ULL; // 20ms in nanoseconds
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
    const auto& s = states_[driver_id];
    const float sector_length = track_.lap_length_km / track_.sectors;
    const float sector_offset = (static_cast<float>(s.sector) - 1.0f) * sector_length;
    return s.lap * track_.lap_length_km + sector_offset + s.distance_in_lap;
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
        frames[driver_id].race_position = static_cast<uint8_t>(i + 1);
    }
}

TelemetryFrame TelemetryGenerator::generateFrame(uint32_t i) {
    auto& state = states_[i];
    const auto& driver = drivers_[i];
    const auto& car = cars_[i];

    float base_threshold = 0.65f + (driver.tire_management * 0.25f);
    float risk_adjustment = (driver.risk_tolerance - 0.5f) * 0.15f;
    float pit_threshold = base_threshold + risk_adjustment;

    bool should_pit = false;
    const bool has_optimal = (optimal_strategies_.find(i) != optimal_strategies_.end());

    if (has_optimal) {
        // If an optimal strategy is provided, follow it exactly (and only once).
        const uint32_t optimal_pit_lap = optimal_strategies_[i];
        should_pit = (state.lap == optimal_pit_lap) && !state.is_on_pit && !state.has_pitted;
    } else {
        // Otherwise pit based on tire wear (can happen multiple times across the race).
        should_pit = (state.tire_wear > pit_threshold) && !state.is_on_pit;
    }

    if (should_pit && !state.is_on_pit) {
        state.is_on_pit = true;
        if (has_optimal) state.has_pitted = true; // consume the planned pit
        state.pit_stop_start_time_ns = current_time_ns_;
        uint64_t pit_duration = static_cast<uint64_t>((2.0f + (1.0f - car.reliability) * 1.0f) * 1e9);
        state.pit_stop_end_time_ns = current_time_ns_ + pit_duration;
    }

    if (state.is_on_pit && current_time_ns_ >= state.pit_stop_end_time_ns) {
        state.is_on_pit = false;
        state.tire_wear = 0.0f;
    }

    float speed = 0.0f;
    if (!state.is_on_pit) {
        float driver_skill = 0.80f + driver.consistency * 0.25f;
        speed = 220.0f * car.engine_power * driver_skill * (1.0f - state.tire_wear * 0.4f);
    }

    if (!state.is_on_pit) {
        constexpr float tick_seconds = 0.02f;
        constexpr float sim_speed_multiplier = 120.0f;
        const float delta_distance_km = speed * (tick_seconds / 3600.0f) * sim_speed_multiplier;

        // Tire wear scales with distance traveled (not per tick), so pit timing stays stable if sim speed changes.
        // Tuned so typical first stops fall roughly in the 15–25 lap range depending on driver traits and track.
        const float wear_per_lap = 0.05f * driver.aggression * track_.tire_wear_factor; // 0..~0.05 per lap
        state.tire_wear += (delta_distance_km / track_.lap_length_km) * wear_per_lap;
        if (state.tire_wear > 1.0f) state.tire_wear = 1.0f;

        state.distance_in_lap += delta_distance_km;

        float sector_length = track_.lap_length_km / track_.sectors;

        while (state.distance_in_lap >= sector_length) {
            state.distance_in_lap -= sector_length;
            state.sector++;

            if (state.sector > track_.sectors) {
                state.sector = 1;
                state.lap++;
            }
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
    float base_temp = state.is_on_pit ? 60.0f : std::clamp(80.0f + speed * 0.05f, 60.0f, 120.0f);
    for(int t = 0; t < 4; t++) {
        frame.tire_temp_c[t] = base_temp;
    }

    return frame;
}

bool TelemetryGenerator::isRaceFinished() const {
    float max_distance = 0;
    uint32_t leader_idx = 0;
    
    for(uint32_t i = 0; i < states_.size(); i++) {
        float total_distance = getTotalDistance(i);
        if(total_distance > max_distance) {
            max_distance = total_distance;
            leader_idx = i;
        }
    }
    
    return states_[leader_idx].lap >= total_laps_;
}

void TelemetryGenerator::setOptimalStrategies(const std::map<uint32_t, uint32_t>& strategies) {
    optimal_strategies_ = strategies;
}