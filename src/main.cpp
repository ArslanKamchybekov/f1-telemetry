#include "ingestion/RingBuffer.h"
#include "telemetry/TelemetryGenerator.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <vector>
#include <atomic>
#include <algorithm>

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
        // MCLAREN
        {.driver_id = "Oscar Piastri", .aggression = 0.74f, .consistency = 0.88f, .tire_management = 0.84f, .risk_tolerance = 0.72f},
        {.driver_id = "Lando Norris", .aggression = 0.80f, .consistency = 0.92f, .tire_management = 0.84f, .risk_tolerance = 0.80f},
        
        // MERCEDES
        {.driver_id = "George Russell", .aggression = 0.75f, .consistency = 0.88f, .tire_management = 0.85f, .risk_tolerance = 0.75f},
        {.driver_id = "Kimi Antonelli", .aggression = 0.80f, .consistency = 0.65f, .tire_management = 0.60f, .risk_tolerance = 0.85f},
        
        // RED BULL RACING
        {.driver_id = "Max Verstappen", .aggression = 0.88f, .consistency = 0.98f, .tire_management = 0.88f, .risk_tolerance = 0.90f},
        {.driver_id = "Yuki Tsunoda", .aggression = 0.73f, .consistency = 0.58f, .tire_management = 0.56f, .risk_tolerance = 0.78f},
        
        // FERRARI
        {.driver_id = "Charles Leclerc", .aggression = 0.94f, .consistency = 0.96f, .tire_management = 0.85f, .risk_tolerance = 0.86f},
        {.driver_id = "Lewis Hamilton", .aggression = 0.66f, .consistency = 0.70f, .tire_management = 0.76f, .risk_tolerance = 0.58f},
        
        // WILLIAMS
        {.driver_id = "Alex Albon", .aggression = 0.70f, .consistency = 0.85f, .tire_management = 0.88f, .risk_tolerance = 0.65f},
        {.driver_id = "Carlos Sainz", .aggression = 0.80f, .consistency = 0.87f, .tire_management = 0.83f, .risk_tolerance = 0.78f},
        
        // RACING BULLS
        {.driver_id = "Liam Lawson", .aggression = 0.82f, .consistency = 0.75f, .tire_management = 0.72f, .risk_tolerance = 0.85f},
        {.driver_id = "Isack Hadjar", .aggression = 0.78f, .consistency = 0.72f, .tire_management = 0.68f, .risk_tolerance = 0.80f},
        
        // ASTON MARTIN
        {.driver_id = "Lance Stroll", .aggression = 0.65f, .consistency = 0.70f, .tire_management = 0.75f, .risk_tolerance = 0.60f},
        {.driver_id = "Fernando Alonso", .aggression = 0.75f, .consistency = 0.92f, .tire_management = 0.95f, .risk_tolerance = 0.85f},
        
        // HAAS
        {.driver_id = "Esteban Ocon", .aggression = 0.73f, .consistency = 0.82f, .tire_management = 0.80f, .risk_tolerance = 0.70f},
        {.driver_id = "Oliver Bearman", .aggression = 0.75f, .consistency = 0.70f, .tire_management = 0.68f, .risk_tolerance = 0.78f},
        
        // KICK SAUBER
        {.driver_id = "Nico Hulkenberg", .aggression = 0.68f, .consistency = 0.85f, .tire_management = 0.88f, .risk_tolerance = 0.65f},
        {.driver_id = "Gabriel Bortoleto", .aggression = 0.73f, .consistency = 0.68f, .tire_management = 0.65f, .risk_tolerance = 0.75f},
        
        // ALPINE
        {.driver_id = "Pierre Gasly", .aggression = 0.75f, .consistency = 0.80f, .tire_management = 0.78f, .risk_tolerance = 0.75f},
        {.driver_id = "Franco Colapinto", .aggression = 0.76f, .consistency = 0.70f, .tire_management = 0.68f, .risk_tolerance = 0.80f},
    };
    
    vector<CarProfile> cars = {
        // MCLAREN
        {.car_id = "McLaren", .engine_power = 0.93f, .aero_efficiency = 0.96f, .cooling_efficiency = 0.93f, .reliability = 0.94f},
        {.car_id = "McLaren", .engine_power = 0.93f, .aero_efficiency = 0.96f, .cooling_efficiency = 0.93f, .reliability = 0.94f},
        
        // MERCEDES - Excellent reliability, good all-rounder
        {.car_id = "Mercedes", .engine_power = 0.92f, .aero_efficiency = 0.94f, .cooling_efficiency = 0.95f, .reliability = 0.96f},
        {.car_id = "Mercedes", .engine_power = 0.92f, .aero_efficiency = 0.94f, .cooling_efficiency = 0.95f, .reliability = 0.96f},
        
        // RED BULL - Still strong but gap closing
        {.car_id = "Red Bull", .engine_power = 0.96f, .aero_efficiency = 0.97f, .cooling_efficiency = 0.90f, .reliability = 0.92f},
        {.car_id = "Red Bull", .engine_power = 0.96f, .aero_efficiency = 0.97f, .cooling_efficiency = 0.90f, .reliability = 0.92f},
        
        // FERRARI - Strong engine, improving aero
        {.car_id = "Ferrari", .engine_power = 0.97f, .aero_efficiency = 0.95f, .cooling_efficiency = 0.86f, .reliability = 0.89f},
        {.car_id = "Ferrari", .engine_power = 0.97f, .aero_efficiency = 0.95f, .cooling_efficiency = 0.86f, .reliability = 0.89f},
        
        // WILLIAMS - Back of midfield
        {.car_id = "Williams", .engine_power = 0.92f, .aero_efficiency = 0.80f, .cooling_efficiency = 0.85f, .reliability = 0.90f},
        {.car_id = "Williams", .engine_power = 0.92f, .aero_efficiency = 0.80f, .cooling_efficiency = 0.85f, .reliability = 0.90f},
        
        // RACING BULLS - Sister team, decent performance
        {.car_id = "Racing Bulls", .engine_power = 0.90f, .aero_efficiency = 0.85f, .cooling_efficiency = 0.87f, .reliability = 0.89f},
        {.car_id = "Racing Bulls", .engine_power = 0.90f, .aero_efficiency = 0.85f, .cooling_efficiency = 0.87f, .reliability = 0.89f},
        
        // ASTON MARTIN - Midfield, improving
        {.car_id = "Aston Martin", .engine_power = 0.88f, .aero_efficiency = 0.87f, .cooling_efficiency = 0.88f, .reliability = 0.90f},
        {.car_id = "Aston Martin", .engine_power = 0.88f, .aero_efficiency = 0.87f, .cooling_efficiency = 0.88f, .reliability = 0.90f},
        
        // HAAS - Lower midfield
        {.car_id = "Haas", .engine_power = 0.90f, .aero_efficiency = 0.82f, .cooling_efficiency = 0.84f, .reliability = 0.88f},
        {.car_id = "Haas", .engine_power = 0.90f, .aero_efficiency = 0.82f, .cooling_efficiency = 0.84f, .reliability = 0.88f},
        
        // KICK SAUBER
        {.car_id = "Kick Sauber", .engine_power = 0.83f, .aero_efficiency = 0.78f, .cooling_efficiency = 0.82f, .reliability = 0.87f},
        {.car_id = "Kick Sauber", .engine_power = 0.83f, .aero_efficiency = 0.78f, .cooling_efficiency = 0.82f, .reliability = 0.87f},
        
        // ALPINE - Struggling midfield
        {.car_id = "Alpine", .engine_power = 0.85f, .aero_efficiency = 0.84f, .cooling_efficiency = 0.86f, .reliability = 0.85f},
        {.car_id = "Alpine", .engine_power = 0.85f, .aero_efficiency = 0.84f, .cooling_efficiency = 0.86f, .reliability = 0.85f},
    };

    uint32_t total_laps = 52;

    RingBuffer<TelemetryFrame> buffer(1024);
    TelemetryGenerator generator(track, drivers, cars, total_laps);

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
        
        while(!done.load()){
            TelemetryFrame frame;
            if(!buffer.pop(frame)) {
                break;
            }
            latestFrames[frame.driver_id] = frame;
            
            static int frameCount = 0;
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
                    for(int i = name.length(); i < 20; i++) cout << " ";
                    
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
                cout << "\033[90mPress Enter to stop the race\033[0m\n";
                cout.flush();
            }
        }
    });

    producer.join();
    consumer.join();

    return 0;
}