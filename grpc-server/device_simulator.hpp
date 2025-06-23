#include <cstdint>
#include <string>

#include "Event.hpp"

struct DeviceData {
    float temperature;   // Temperature in Celsius
    float humidity;      // Humidity in percentage
    float pressure;      // Pressure in hPa
    int64_t timestamp;   // Timestamp in milliseconds since epoch
};

struct EventData {
    std::string device_name; // Name of the device
    DeviceData data; 
};

class DeviceSimulator {
public:
    Event<EventData> onDataChanged;
    void addDevice(std::string device_name);
private:
    std::vector<std::string> devices; // List of devices
    void simulate(std::string device_name);
    DeviceData generateRandomDeviceData();
};
