#pragma once

#include "../common/types.h"
#include <vector>

namespace SeasonData {
    // 2025 F1 Season Driver Profiles
    const std::vector<DriverProfile> DRIVERS = {
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

    // 2025 F1 Season Car Profiles
    const std::vector<CarProfile> CARS = {
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
}

