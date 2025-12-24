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
                cout << "\n🏁 RACE FINISHED! 🏁\n";
                cout << "Winner: " << drivers[0].driver_id << "\n";

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
        TelemetryFrame frame;
        while(!done.load()){
            if(buffer.pop(frame)){
                cout << "[Telemetry] "
                << "P" << int(frame.race_position) << " "
                << drivers[frame.driver_id].driver_id
                << " Lap " << frame.lap
                << " Sector "<< int(frame.sector)
                << " Speed "<< frame.speed_kph;

                if (frame.speed_kph == 0.0f && frame.lap > 0) {
                    cout << " [IN PITS]";
                } 

                cout << " Tire: " << (frame.tire_wear * 100) << "%" << "\n";
            } else {
                this_thread::sleep_for(chrono::milliseconds(1)); // to sleep the thread to avoid busy-waiting
            }
        }
    });

    cin.get();

    done.store(true);

    producer.join();
    consumer.join();

    return 0;
}