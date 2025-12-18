#pragma once

#include <string>
#include <vector>
#include <memory>
#include "CelestialBody.hpp"

namespace Simulation {

struct SystemData {
    std::string name;
    float systemScale = 10.0f;
    float planetScale = 1.0f;
    std::vector<std::unique_ptr<CelestialBody>> bodies;
};

class SystemLoader {
public:
    // Load system from JSON file
    // Returns nullptr if file not found or parse error
    static std::unique_ptr<SystemData> loadFromFile(const std::string& filePath);
    
    // Get the file path for a system name
    static std::string getSystemFilePath(const std::string& systemName);
    
    // List available system names
    static std::vector<std::string> getAvailableSystems();

};

} // namespace Simulation
