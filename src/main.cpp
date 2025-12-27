#include "ingestion/RingBuffer.h"
#include "telemetry/TelemetryGenerator.h"
#include "strategy/StrategyAnalyzer.h"
#include "data/season_data.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <vector>
#include <atomic>
#include <algorithm>
#include <sstream>

using namespace std;

vector<uint32_t> parseDriverIds(const string& input, size_t max_id){
    vector<uint32_t> driver_ids;
    stringstream ss(input);
    string id;
    while(getline(ss, id, ',')){
        if(id.empty()) continue;
        try {
            int val = stoi(id);
            if(val >= 0 && static_cast<size_t>(val) < max_id) {
                driver_ids.push_back(static_cast<uint32_t>(val));
            }
        } catch(...) {
            // Skip invalid entries
        }
    }
    return driver_ids;
}

int main(){

    atomic<bool> done(false);

    TrackProfile track = {
        .track_id = 1,
        .sectors = 3,
        .lap_length_km = 10.0f,
        // Baseline wear multiplier (1.0 ~= "normal"). Higher => earlier pit windows.
        .tire_wear_factor = 1.0f,
        .overtaking_difficulty = 0.1f,
        .safety_car_probability = 0.01f,
    };
    
    // Load 2025 F1 season data
    const vector<DriverProfile>& drivers = SeasonData::DRIVERS;
    const vector<CarProfile>& cars = SeasonData::CARS;

    uint32_t total_laps = 52;

    // Ask user about strategy optimization
    cout << "\nRun strategy analysis? (y/n): ";
    string response;
    getline(cin, response);

    map<uint32_t, uint32_t> optimal_strategies;

    if (response == "y" || response == "Y") {
        // Display driver list
        cout << "\nAvailable drivers:\n";
        for(uint32_t i = 0; i < drivers.size(); i++) {
            cout << i << ": " << drivers[i].driver_id << "\n";
        }
        
        cout << "\nEnter driver IDs to optimize (comma-separated, no spaces): ";
        string input;
        getline(cin, input);
        
        vector<uint32_t> driver_ids = parseDriverIds(input, drivers.size());
        
        if(driver_ids.empty()) {
            cout << "No valid driver IDs entered. Skipping strategy analysis.\n";
        } else {
        
        cout << "\nAnalyzing strategies (this may take 10-30 seconds)...\n";
        
        // Run strategy analyzer
        StrategyAnalyzer analyzer(track, drivers, cars, total_laps);
        vector<StrategyResult> results = analyzer.analyzeStrategies(driver_ids);
        
        // Display results
        cout << "\nStrategy Analysis Results:\n";
        cout << "==========================\n";
        for(const auto& result : results) {
            cout << drivers[result.driver_id].driver_id 
                << ": Pit lap " << result.optimal_pit_lap 
                << " (finish time: " << (result.finish_time_seconds / 60.0f) << " min)\n";
            
            // Store for use in live race
            optimal_strategies[result.driver_id] = result.optimal_pit_lap;
        }
        cout << "\n";
        } // end else block for non-empty driver_ids
    }

    RingBuffer<TelemetryFrame> buffer(1024);
    TelemetryGenerator generator(track, drivers, cars, total_laps);

    if(!optimal_strategies.empty()) {
        generator.setOptimalStrategies(optimal_strategies);
    }

    // Print strategies that will be used, then start the race
    cout << "\nRace strategies:\n";
    cout << "================\n";
    for (uint32_t i = 0; i < drivers.size(); i++) {
        cout << drivers[i].driver_id << ": ";
        auto it = optimal_strategies.find(i);
        if (it != optimal_strategies.end()) {
            cout << "Optimal pit lap " << it->second << "\n";
        } else {
            cout << "Wear-based pitting\n";
        }
    }
    cout << "\nPress Enter to start race...\n";
    cout.flush();
    {
        string start;
        getline(cin, start);
    }
    cout << "\nStarting race...\n\n";

    thread producer([&]() {
        while(!done.load()){
            auto frames = generator.next();

            if(generator.isRaceFinished()) {
                done.store(true);
                buffer.shutdown();
                
                string winner = "";
                for(const auto& frame : frames) {
                    if(frame.race_position == 1) {
                        winner = drivers[frame.driver_id].driver_id;
                        break;
                    }
                }
                
                cout << "\n🏁 RACE FINISHED! 🏁\n";
                cout << "🏆 Winner: " << winner << " 🏆\n";

                break;
            }

            for(const auto &frame : frames){
                if(!buffer.push(frame)){
                    TelemetryFrame old_frame;
                    buffer.pop(old_frame);
                    cout << "[Telemetry] Buffer full, dropping frame " << drivers[old_frame.driver_id].driver_id << "\n";
                    buffer.push(frame);
                }
            }
            this_thread::sleep_for(chrono::milliseconds(20));
        }
    });

    thread consumer([&]() {
        vector<TelemetryFrame> latestFrames(drivers.size());
        // Initialize with valid positions to avoid sorting issues on first frames
        for(size_t i = 0; i < latestFrames.size(); i++) {
            latestFrames[i].driver_id = static_cast<uint32_t>(i);
            latestFrames[i].race_position = static_cast<uint8_t>(i + 1);
            latestFrames[i].lap = 0;
            latestFrames[i].sector = 1;
        }
        
        while(!done.load()){
            TelemetryFrame frame;
            if(!buffer.pop(frame)) {
                break;
            }
            latestFrames[frame.driver_id] = frame;
            
            static size_t frameCount = 0;
            frameCount++;
            
            if(frameCount % drivers.size() == 0) {
                cout << "\033[2J\033[H";
                
                uint32_t currentLap = 0;
                for(const auto& f : latestFrames) {
                    if(f.lap > currentLap) currentLap = f.lap;
                }
                
                cout << "\n🏁 LAP " << currentLap << "/" << total_laps << " 🏁\n";
                cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
                
                vector<TelemetryFrame> sortedFrames = latestFrames;
                sort(sortedFrames.begin(), sortedFrames.end(), 
                        [](const TelemetryFrame& a, const TelemetryFrame& b) {
                            return a.race_position < b.race_position;
                        });
                
                for(const auto& f : sortedFrames) {
                    string posColor = "\033[1;33m";
                    if(f.race_position == 1) posColor = "\033[1;93m";
                    else if(f.race_position == 2) posColor = "\033[1;37m";
                    else if(f.race_position == 3) posColor = "\033[1;91m";
                    
                    cout << posColor << "P" << int(f.race_position);
                    if(f.race_position < 10) cout << " ";
                    cout << "\033[0m ";
                    
                    string emoji = "⚫";
                    string teamName = cars[f.driver_id].car_id;
                    if(teamName == "Red Bull") emoji = "🔵";
                    else if(teamName == "Ferrari") emoji = "🔴";
                    else if(teamName == "Mercedes") emoji = "⚪";
                    else if(teamName == "McLaren") emoji = "🟠";
                    else if(teamName == "Aston Martin") emoji = "🟢";
                    else if(teamName == "Alpine") emoji = "💙";
                    else if(teamName == "Haas") emoji = "⚪";
                    else if(teamName == "Racing Bulls") emoji = "🔷";
                    else if(teamName == "Williams") emoji = "💙";
                    else if(teamName == "Kick Sauber") emoji = "🟢";
                    
                    cout << emoji << " ";
                    
                    string name = drivers[f.driver_id].driver_id;
                    cout << "\033[1m" << name << "\033[0m";
                    for(size_t i = name.length(); i < 20; i++) cout << " ";
                    
                    int barLength = 10;
                    float progress = (f.sector - 1) / float(track.sectors);
                    int filled = int(progress * barLength);
                    cout << " ";
                    for(int i = 0; i < barLength; i++) {
                        if(i < filled) cout << "█";
                        else cout << "░";
                    }
                    
                    cout << " Lap " << f.lap;
                    
                    if(f.speed_kph == 0.0f) {
                        cout << "  \033[1;35m[IN PITS]\033[0m";
                    } else {
                        string speedColor = "\033[32m";
                        if(f.speed_kph < 150) speedColor = "\033[31m";
                        else if(f.speed_kph < 200) speedColor = "\033[33m";
                        
                        cout << "  Speed: " << speedColor << int(f.speed_kph) << " kph\033[0m";
                    }
                    
                    float tirePercent = f.tire_wear * 100;
                    string tireColor = "\033[32m";
                    if(tirePercent > 70) tireColor = "\033[31m";
                    else if(tirePercent > 40) tireColor = "\033[33m";
                    
                    cout << "  Tire: " << tireColor << int(tirePercent) << "%\033[0m";
                    
                    cout << "\n";
                }
                
                cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
                cout << "\033[90mRace runs until finish\033[0m\n";
                cout.flush();
            }
        }
    });

    producer.join();
    consumer.join();

    return 0;
}