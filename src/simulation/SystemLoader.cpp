#include "SystemLoader.hpp"
#include "third_party/json.hpp"
#include "core/Logger.hpp"
#include <fstream>
#include <filesystem>
#include <algorithm>

namespace Simulation {

using json = nlohmann::json;

// Helper function to parse BodyType from string
BodyType parseBodyType(const std::string& typeStr) {
    static const std::unordered_map<std::string, BodyType> typeMap = {
        {"star", BodyType::Star},
        {"planet", BodyType::Planet},
        {"moon", BodyType::Moon},
        {"dwarf_planet", BodyType::DwarfPlanet},
        {"asteroid", BodyType::Asteroid},
        {"comet", BodyType::Comet},
        {"black_hole", BodyType::BlackHole},
        {"other", BodyType::Other}
    };
    
    auto it = typeMap.find(typeStr);
    return (it != typeMap.end()) ? it->second : BodyType::Planet;
}

std::unique_ptr<CelestialBody> parseBody(const json& bodyJson, bool isChild = false) {
    std::string name = bodyJson.at("name").get<std::string>();
    double radius = bodyJson.at("radius").get<double>();
    
    auto colorArr = bodyJson.at("color");
    glm::vec3 color(
        colorArr[0].get<float>(),
        colorArr[1].get<float>(),
        colorArr[2].get<float>()
    );
    
    auto orbitJson = bodyJson.at("orbit");
    OrbitalParams orbit{
        orbitJson.at("semiMajorAxis").get<double>(),
        orbitJson.at("eccentricity").get<double>(),
        glm::radians(orbitJson.at("inclination").get<double>()),
        orbitJson.at("period").get<double>(),
        glm::radians(orbitJson.at("meanAnomaly").get<double>()),
        glm::radians(orbitJson.at("longitudeAscendingNode").get<double>()),
        glm::radians(orbitJson.at("argumentPeriapsis").get<double>())
    };
    
    // Determine body type
    BodyType bodyType = BodyType::Planet;
    if (bodyJson.contains("type")) {
        bodyType = parseBodyType(bodyJson.at("type").get<std::string>());
    } else if (isChild) {
        bodyType = BodyType::Moon;  // Default children to moons
    }
    
    auto body = std::make_unique<CelestialBody>(name, radius, color, orbit, bodyType);
    
    // Parse mass if provided, else estimate from radius
    if (bodyJson.contains("mass")) {
        body->setMass(bodyJson.at("mass").get<double>());
    } else {
        body->setMass(radius * radius * radius);
    }
    
    // Parse children (moons)
    if (bodyJson.contains("children")) {
        for (const auto& childJson : bodyJson.at("children")) {
            body->addChild(parseBody(childJson, true));
        }
    }
    
    return body;
}

std::unique_ptr<SystemData> SystemLoader::loadFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        LOG_ERROR("SystemLoader", "Failed to open file: ", filePath);
        return nullptr;
    }
    
    try {
        json j = json::parse(file);
        
        auto data = std::make_unique<SystemData>();
        data->name = j.at("name").get<std::string>();
        data->systemScale = j.at("scale").at("system").get<float>();
        data->planetScale = j.at("scale").at("planet").get<float>();
        
        for (const auto& bodyJson : j.at("bodies")) {
            data->bodies.push_back(parseBody(bodyJson));
        }
        
        LOG_INFO("SystemLoader", "Loaded system '", data->name, "' with ", data->bodies.size(), " bodies");
        
        return data;
    } catch (const json::exception& e) {
        LOG_ERROR("SystemLoader", "JSON parse error in ", filePath, ": ", e.what());
        return nullptr;
    }
}

std::string SystemLoader::getSystemFilePath(const std::string& systemName) {
    // Map display names to file names
    static const std::unordered_map<std::string, std::string> nameToFile = {
        {"Solar System", "solar_system.json"},
        {"TRAPPIST-1", "trappist1.json"},
        {"Kepler-90", "kepler90.json"},
        {"HR 8799", "hr8799.json"},
        {"Kepler-11", "kepler11.json"},
        {"55 Cancri", "55cancri.json"}
    };
    
    auto it = nameToFile.find(systemName);
    if (it != nameToFile.end()) {
        return "assets/systems/" + it->second;
    }
    
    // Fallback: convert name to lowercase, replace spaces with underscores
    std::string fileName = systemName;
    std::transform(fileName.begin(), fileName.end(), fileName.begin(), ::tolower);
    std::replace(fileName.begin(), fileName.end(), ' ', '_');
    return "assets/systems/" + fileName + ".json";
}

std::vector<std::string> SystemLoader::getAvailableSystems() {
    return {
        "Solar System",
        "TRAPPIST-1",
        "Kepler-90",
        "Kepler-11",
        "55 Cancri",
        "HR 8799",
        "Black Hole",
        "Binary Star"
    };
}

} // namespace Simulation
