#include "ingestion/RingBuffer.h"
#include "telemetry/TelemetryGenerator.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <atomic>
#include <mutex>
#include <atomic>

using namespace std;

int main(){

    atomic<bool> done(false);

    TrackProfile track = {
        .track_id = 1,
        .sectors = 3,
        .lap_length_km = 10.0f,
        .tire_wear_factor = 10.0f,
        .overtaking_difficulty = 0.1f,
        .safety_car_probability = 0.01f,
    };
    vector<DriverProfile> drivers = {
        // Max Verstappen - Aggressive, consistent, good tire management, high risk tolerance
        {.driver_id = "Max Verstappen", .aggression = 0.85f, .consistency = 0.90f, .tire_management = 0.80f, .risk_tolerance = 0.90f},
        // Lewis Hamilton - Balanced, extremely consistent, excellent tire management, moderate risk
        {.driver_id = "Lewis Hamilton", .aggression = 0.65f, .consistency = 0.95f, .tire_management = 0.95f, .risk_tolerance = 0.60f},
        // Charles Leclerc - Very aggressive, moderate consistency, moderate tire management, high risk
        {.driver_id = "Charles Leclerc", .aggression = 0.90f, .consistency = 0.75f, .tire_management = 0.70f, .risk_tolerance = 0.85f},
    };
    vector<CarProfile> cars = {
        // Red Bull - High power, excellent aero, good cooling, very reliable
        {.car_id = "Red Bull", .engine_power = 0.95f, .aero_efficiency = 0.98f, .cooling_efficiency = 0.88f, .reliability = 0.92f},
        // Mercedes - Good power, excellent aero, excellent cooling, very reliable
        {.car_id = "Mercedes", .engine_power = 0.90f, .aero_efficiency = 0.95f, .cooling_efficiency = 0.95f, .reliability = 0.98f},
        // Ferrari - High power, good aero, moderate cooling, moderate reliability
        {.car_id = "Ferrari", .engine_power = 0.98f, .aero_efficiency = 0.92f, .cooling_efficiency = 0.80f, .reliability = 0.85f},
    };
    uint32_t total_laps = 50;

    RingBuffer<TelemetryFrame> buffer(1024);
    TelemetryGenerator generator(track, drivers, cars, total_laps);

    thread producer([&]() {
        while(!done.load()){
            auto frames = generator.next();

            if(generator.isRaceFinished()) {
                done.store(true);
                
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
        uint32_t lastDisplayedLap = 0;
        
        while(!done.load()){
            TelemetryFrame frame;
            if(buffer.pop(frame)){
                latestFrames[frame.driver_id] = frame;
                
                // Display full leaderboard every time we get all frames
                static int frameCount = 0;
                frameCount++;
                
                if(frameCount % drivers.size() == 0) {
                    // Clear screen and move cursor to top
                    cout << "\033[2J\033[H";
                    
                    // Get current lap (from first driver)
                    uint32_t currentLap = latestFrames[0].lap;
                    
                    // Header
                    cout << "\n🏁 LAP " << currentLap << "/" << total_laps << " 🏁\n";
                    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
                    
                    // Sort by position
                    vector<TelemetryFrame> sortedFrames = latestFrames;
                    sort(sortedFrames.begin(), sortedFrames.end(), 
                         [](const TelemetryFrame& a, const TelemetryFrame& b) {
                             return a.race_position < b.race_position;
                         });
                    
                    // Display each driver
                    for(const auto& f : sortedFrames) {
                        // Position with color
                        string posColor = "\033[1;33m"; // Yellow
                        if(f.race_position == 1) posColor = "\033[1;93m"; // Bright yellow (gold)
                        else if(f.race_position == 2) posColor = "\033[1;37m"; // White (silver)
                        else if(f.race_position == 3) posColor = "\033[1;91m"; // Bright red (bronze)
                        
                        cout << posColor << "P" << int(f.race_position) << "\033[0m ";
                        
                        // Team emoji
                        string emoji = "🔴";
                        if(cars[f.driver_id].car_id == "Red Bull") emoji = "🔵";
                        else if(cars[f.driver_id].car_id == "Mercedes") emoji = "⚪";
                        
                        cout << emoji << " ";
                        
                        // Driver name (padded)
                        string name = drivers[f.driver_id].driver_id;
                        cout << "\033[1m" << name << "\033[0m";
                        for(int i = name.length(); i < 20; i++) cout << " ";
                        
                        // Progress bar (lap completion)
                        int barLength = 10;
                        float progress = (f.sector - 1) / float(track.sectors);
                        int filled = int(progress * barLength);
                        cout << " ";
                        for(int i = 0; i < barLength; i++) {
                            if(i < filled) cout << "█";
                            else cout << "░";
                        }
                        
                        // Lap info
                        cout << " Lap " << f.lap;
                        
                        // Speed with color
                        if(f.speed_kph == 0.0f) {
                            cout << "  \033[1;35m[IN PITS]\033[0m";
                        } else {
                            string speedColor = "\033[32m"; // Green
                            if(f.speed_kph < 150) speedColor = "\033[31m"; // Red
                            else if(f.speed_kph < 200) speedColor = "\033[33m"; // Yellow
                            
                            cout << "  Speed: " << speedColor << int(f.speed_kph) << " kph\033[0m";
                        }
                        
                        // Tire wear with color
                        float tirePercent = f.tire_wear * 100;
                        string tireColor = "\033[32m"; // Green
                        if(tirePercent > 70) tireColor = "\033[31m"; // Red
                        else if(tirePercent > 40) tireColor = "\033[33m"; // Yellow
                        
                        cout << "  Tire: " << tireColor << int(tirePercent) << "%\033[0m";
                        
                        cout << "\n";
                    }
                    
                    cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
                    cout << "\033[90mPress Enter to stop the race\033[0m\n";
                    cout.flush();
                }
            } else {
                this_thread::sleep_for(chrono::milliseconds(1));
            }
        }
    });

    cin.get();

    done.store(true);

    producer.join();
    consumer.join();

    return 0;
}