#include "device_simulator.hpp"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <unistd.h>


void DeviceSimulator::addDevice(std::string device_name) {
    devices.push_back(device_name);
    std::cout << "Device added: " << devices.back() << std::endl;
    // Start simulating data for the new device
    
    simulate(device_name);
}

void DeviceSimulator::simulate(std::string device_name) {
    // Implementation for simulating device events
    // This could be a loop that generates random data at intervals
    while (true) {
        DeviceData data = generateRandomDeviceData();
        // Here you would typically send this data to a server or process it
        // std::cout << "Simulated Data: "
        //           << "Device: " << device_name
        //           << "  Temperature: " << data.temperature
        //           << "  Humidity: " << data.humidity
        //           << "  Pressure: " << data.pressure
        //           << "  Timestamp: " << data.timestamp << std::endl;
        onDataChanged.trigger(EventData{device_name: device_name, data: data});  // Trigger the event with the new data
        sleep( 1 + std::rand() % 5 );  // Simulate a delay between events between 1 to 5 seconds
    }
}

DeviceData DeviceSimulator::generateRandomDeviceData() {
    // Implementation for simulating a device event
    return DeviceData{
        temperature: static_cast<float>(rand() % 100),  // Random temperature
        humidity: static_cast<float>(rand() % 100),     // Random humidity
        pressure: static_cast<float>(rand() % 1000 + 900), // Random pressure
        timestamp: std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count()
    };
}