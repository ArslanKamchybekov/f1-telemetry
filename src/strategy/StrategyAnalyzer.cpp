#include "StrategyAnalyzer.h"

using namespace std;

StrategyAnalyzer::StrategyAnalyzer(
    const TrackProfile& track,
    const vector<DriverProfile>& drivers,
    const vector<CarProfile>& cars,
    uint32_t total_laps
) : track_(track), drivers_(drivers), cars_(cars), total_laps_(total_laps) {}

vector<StrategyResult> StrategyAnalyzer::analyzeStrategies(const std::vector<uint32_t>& driver_ids_to_optimize) {

}

StrategyResult StrategyAnalyzer::findOptimalForDriver(uint32_t driver_id) {
    
}