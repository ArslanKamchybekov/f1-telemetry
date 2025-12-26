#include "RaceSimulator.h"

using namespace std;

RaceSimulator::RaceSimulator(
    const TrackProfile& track, 
    const vector<DriverProfile>& drivers, 
    const vector<CarProfile>& cars, 
    uint32_t total_laps
) : track_(track), drivers_(drivers), cars_(cars), total_laps_(total_laps) {
    states_.resize(drivers.size());
    for(auto &s : states_) {
        s.lap = 0;
        s.sector = 0;
        s.tire_wear = 0.0f;
        s.distance_in_lap = 0.0f;
        s.total_time_seconds = 0.0f;
        s.has_pitted = false;
        s.is_on_pit = false;
    }
}

bool RaceSimulator::shouldPit(uint32_t driver_id, uint32_t target_driver_id, uint32_t forced_pit_lap) {
    const auto &driver = drivers_[driver_id];
    const auto &state = states_[driver_id];

    if(driver_id == target_driver_id) {
        return state.lap == forced_pit_lap && !state.has_pitted;
    } else {
        float base_threshold = 0.65f + (driver.tire_management * 0.25f);
        float risk_adjustment = (driver.risk_tolerance - 0.5f) * 0.15f;
        float pit_threshold = base_threshold + risk_adjustment;
        
        return state.tire_wear > pit_threshold && !state.has_pitted;
    }
}

void RaceSimulator::updateDriverState(uint32_t driver_id, uint32_t target_driver_id, uint32_t forced_pit_lap) {
    constexpr float tick_seconds = 0.02f;

    auto &state = states_[driver_id];
    const auto &driver = drivers_[driver_id];
    const auto &car = cars_[driver_id];

    if (shouldPit(driver_id, target_driver_id, forced_pit_lap)) {
        state.has_pitted = true;
        state.is_on_pit = true;
        state.total_time_seconds += (2.0f + (1.0f - car.reliability) * 1.0f);;
        state.tire_wear = 0.0f;
        return;
    }

    float speed = 0.0f;
    if (!state.is_on_pit) {
        float driver_skill = 0.80f + driver.consistency * 0.25f;
        speed = 220.0f * car.engine_power * driver_skill * (1.0f - state.tire_wear * 0.4f);
    }

    if (!state.is_on_pit) {
        state.tire_wear += 0.0005f * track_.tire_wear_factor * driver.aggression;
        if (state.tire_wear > 1.0f) state.tire_wear = 1.0f;
    }

    if (!state.is_on_pit) {
        state.distance_in_lap += speed * 0.005f;

        float sector_length = track_.lap_length_km / track_.sectors;

        if (state.distance_in_lap >= sector_length) {
            state.distance_in_lap = state.distance_in_lap - sector_length;
            state.sector++;

            if (state.sector > track_.sectors) {
                state.sector = 1;
                state.lap++;
            }
        }
    }

    state.total_time_seconds += tick_seconds;
} 

void RaceSimulator::simulateTick(uint32_t target_driver_id, uint32_t pit_lap) {
    for(uint32_t i = 0; i < drivers_.size(); i++) {
        updateDriverState(i, target_driver_id, pit_lap);
    }
}

float RaceSimulator::simulateRace(uint32_t target_driver_id, uint32_t pit_lap) {
    for(auto &s : states_){
        s.lap = 0;
        s.sector = 0;
        s.tire_wear = 0.0f;
        s.distance_in_lap = 0.0f;
        s.total_time_seconds = 0.0f;
        s.has_pitted = false;
        s.is_on_pit = false;
    }

    while(states_[target_driver_id].lap < total_laps_) {
        simulateTick(target_driver_id, pit_lap);
    }

    return states_[target_driver_id].total_time_seconds;
}