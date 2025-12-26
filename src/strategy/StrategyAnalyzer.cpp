#include "StrategyAnalyzer.h"
#include <future>

using namespace std;

const vector<uint32_t> StrategyAnalyzer::PIT_LAPS_TO_TEST = {12, 15, 18, 21, 24, 27, 30, 33, 36, 39};

StrategyAnalyzer::StrategyAnalyzer(
    const TrackProfile& track,
    const vector<DriverProfile>& drivers,
    const vector<CarProfile>& cars,
    uint32_t total_laps
) : track_(track), drivers_(drivers), cars_(cars), total_laps_(total_laps) {}

vector<StrategyResult> StrategyAnalyzer::analyzeStrategies(const std::vector<uint32_t>& driver_ids_to_optimize) {
    vector<StrategyResult> results;

    for(uint32_t i = 0; i < driver_ids_to_optimize.size(); i++) {
        uint32_t driver_id = driver_ids_to_optimize[i];
        StrategyResult result = findOptimalForDriver(driver_id);
        results.push_back(result);
    }

    return results;
}

StrategyResult StrategyAnalyzer::findOptimalForDriver(uint32_t driver_id) {
    vector<future<float>> futures;

    for(uint32_t i = 0; i < PIT_LAPS_TO_TEST.size(); i++) {
        futures.push_back(
            async(launch::async, [=](){
                RaceSimulator simulator(track_, drivers_, cars_, total_laps_);
                return simulator.simulateRace(driver_id, PIT_LAPS_TO_TEST[i]);
            })
        );
    }

    uint32_t best_pit_lap = PIT_LAPS_TO_TEST[0];
    float best_time = futures[0].get();

    for(uint32_t i = 1; i < PIT_LAPS_TO_TEST.size(); i++) {
        float time = futures[i].get();
        
        if(time < best_time) {
            best_time = time;
            best_pit_lap = PIT_LAPS_TO_TEST[i];
        }
    }

    return {driver_id, best_pit_lap, best_time};
}