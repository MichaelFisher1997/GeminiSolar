#include "SimulationUI.hpp"
#include "imgui.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Render {

SimulationUI::SimulationUI(Platform::WindowInterface& window)
    : m_window(window) {
}

void SimulationUI::render(
    Simulation::SolarSystem& solarSystem,
    Core::Time& time,
    [[maybe_unused]] const Camera& camera,
    [[maybe_unused]] const Simulation::CelestialBody* hoveredBody,
    [[maybe_unused]] const Simulation::CelestialBody* selectedBody,
    const Simulation::CelestialBody* lockedBody,
    const UICallbacks& callbacks
) {
    renderStatsOverlay(time);
    renderControlPanel(solarSystem, time, callbacks);
    renderBodiesPanel(solarSystem, time, lockedBody, callbacks);
    renderHelpButton();
}

void SimulationUI::renderStatsOverlay(const Core::Time& time) {
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.35f);
    if (ImGui::Begin("Stats", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove)) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Solar System Simulator");
        ImGui::Separator();
        ImGui::Text("Time: %.2f years", time.getSimulationTime());
        ImGui::Text("FPS:  %.0f", ImGui::GetIO().Framerate);
        if (time.isPaused()) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "[PAUSED]");
        }
    }
    ImGui::End();
}

void SimulationUI::renderControlPanel(
    Simulation::SolarSystem& solarSystem,
    Core::Time& time,
    const UICallbacks& callbacks
) {
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 310, 10), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(300, 0), ImGuiCond_Always);
    if (ImGui::Begin("Simulation Controls", nullptr)) {
        if (ImGui::CollapsingHeader("System Selection", ImGuiTreeNodeFlags_DefaultOpen)) {
            // Use SystemLoader to get available systems (OCP compliance)
            auto allSystems = Simulation::SystemLoader::getAvailableSystems();
            
            // Split into planetary systems and special events
            std::vector<std::string> planetSystems;
            std::vector<std::string> specialEvents;
            for (const auto& name : allSystems) {
                if (name == "Black Hole" || name == "Binary Star") {
                    specialEvents.push_back(name);
                } else {
                    planetSystems.push_back(name);
                }
            }

            ImGui::TextDisabled("Planetary Systems");
            ImGui::PushID("Planets");
            for (const auto& name : planetSystems) {
                bool isSelected = (solarSystem.getCurrentSystemName() == name);
                if (ImGui::Selectable(name.c_str(), isSelected)) {
                    if (callbacks.onSystemSelected) {
                        callbacks.onSystemSelected(name);
                    }
                }
            }
            ImGui::PopID();

            ImGui::Separator();
            ImGui::TextDisabled("Special Events");
            ImGui::PushID("Events");
            for (const auto& name : specialEvents) {
                bool isSelected = (solarSystem.getCurrentSystemName() == name);
                if (ImGui::Selectable(name.c_str(), isSelected)) {
                    if (callbacks.onSystemSelected) {
                        callbacks.onSystemSelected(name);
                    }
                }
            }
            ImGui::PopID();
        }

        if (ImGui::CollapsingHeader("Time Controls", ImGuiTreeNodeFlags_DefaultOpen)) {
            const double DAY_IN_YEARS = 1.0 / 365.25;
            float multiplier = static_cast<float>(time.getTimeScale() / DAY_IN_YEARS);
            if (ImGui::SliderFloat("Speed (Days/s)", &multiplier, 0.0f, 400.0f, "%.4f", ImGuiSliderFlags_Logarithmic)) {
                time.setTimeScale(static_cast<double>(multiplier) * DAY_IN_YEARS);
            }
            if (ImGui::Button(time.isPaused() ? "Resume (P)" : "Pause (P)", ImVec2(-1, 0))) {
                time.setPaused(!time.isPaused());
            }
        }

        if (ImGui::CollapsingHeader("Visual Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::Checkbox("Show Orbital Rings", &m_showOrbits)) {}
            if (ImGui::Checkbox("Show Planet Labels", &m_showLabels)) {}

            bool physicsEnabled = solarSystem.isPhysicsEnabled();
            if (ImGui::Checkbox("N-Body Gravity (Chaos)", &physicsEnabled)) {
                solarSystem.setPhysicsEnabled(physicsEnabled);
            }
        }

        if (solarSystem.getCurrentSystemName() == "Solar System") {
            if (ImGui::CollapsingHeader("Historic Presets")) {
                auto eventBtn = [&time, &callbacks](const char* label, double eventTime) {
                    if (ImGui::Button(label, ImVec2(-1, 0))) {
                        time.setSimulationTime(eventTime);
                        time.setPaused(true);
                        if (callbacks.onClearFocus) {
                            callbacks.onClearFocus();
                        }
                    }
                };
                eventBtn("Great Conjunction (2020)", 20.972);
                eventBtn("Total Solar Eclipse (2017)", 17.638);
                eventBtn("Venus Transit (2012)", 12.427);
                eventBtn("Shoemaker-Levy 9 (1994)", -5.460);
                eventBtn("Voyager 2 Neptune (1989)", -10.353);
            }
        }
    }
    ImGui::End();
}

void SimulationUI::renderBodiesPanel(
    Simulation::SolarSystem& solarSystem,
    [[maybe_unused]] const Core::Time& time,
    const Simulation::CelestialBody* lockedBody,
    const UICallbacks& callbacks
) {
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 310, ImGui::GetIO().DisplaySize.y - 260), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(300, 250), ImGuiCond_Always);
    if (ImGui::Begin("Celestial Bodies", nullptr)) {
        if (ImGui::Button("Clear Focus", ImVec2(-1, 0))) {
            if (callbacks.onClearFocus) {
                callbacks.onClearFocus();
            }
        }
        ImGui::Separator();
        ImGui::BeginChild("BodyList");
        
        const auto* sun = solarSystem.getSun();
        if (sun) {
            bool isSelected = (lockedBody == sun);
            if (ImGui::Selectable(sun->getName().c_str(), isSelected)) {
                if (callbacks.onBodySelected) {
                    callbacks.onBodySelected(sun);
                }
            }
        }

        auto& bodies = solarSystem.getBodies();
        for (size_t i = 1; i < bodies.size(); ++i) {
            const auto& body = bodies[i];
            ImGui::PushID(body.get());
            bool isSelected = (lockedBody == body.get());
            if (ImGui::Selectable(body->getName().c_str(), isSelected)) {
                if (callbacks.onBodySelected) {
                    callbacks.onBodySelected(body.get());
                }
            }
            for (const auto& child : body->getChildren()) {
                ImGui::PushID(child.get());
                bool childSelected = (lockedBody == child.get());
                if (ImGui::Selectable(("  > " + child->getName()).c_str(), childSelected)) {
                    if (callbacks.onBodySelected) {
                        callbacks.onBodySelected(child.get());
                    }
                }
                ImGui::PopID();
            }
            ImGui::PopID();
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

void SimulationUI::renderHelpButton() {
    ImGui::SetNextWindowPos(ImVec2(10, ImGui::GetIO().DisplaySize.y - 40), ImGuiCond_Always);
    if (ImGui::Begin("HelpToggle", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground)) {
        if (ImGui::Button("? Controls Help")) m_showHelp = !m_showHelp;
    }
    ImGui::End();
    
    if (m_showHelp) {
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        if (ImGui::Begin("Controls Help", &m_showHelp, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::BulletText("Right-Click + Drag: Rotate");
            ImGui::BulletText("Scroll Wheel: Zoom");
            ImGui::BulletText("Middle-Click: Pan");
            ImGui::Separator();
            ImGui::BulletText("Tab: Camera Mode");
            ImGui::BulletText("Double-Click Planet: Focus");
            ImGui::BulletText("P: Pause | O: Reset View");
            if (ImGui::Button("Close", ImVec2(-1, 0))) m_showHelp = false;
        }
        ImGui::End();
    }
}

void SimulationUI::renderLabels(
    const Simulation::SolarSystem& solarSystem,
    const Camera& camera,
    double simulationTime,
    const Simulation::CelestialBody* hoveredBody,
    const Simulation::CelestialBody* selectedBody,
    const Simulation::CelestialBody* lockedBody,
    bool showLabels
) {
    if (!showLabels) return;
    
    int w, h;
    m_window.getSize(w, h);
    float sw = static_cast<float>(w), sh = static_cast<float>(h);
    glm::mat4 viewProj = camera.getProjectionMatrix() * camera.getViewMatrix();
    float sysScale = solarSystem.getSystemScale();
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    
    std::function<void(const Simulation::CelestialBody&, glm::vec3)> drawLabel;
    drawLabel = [&](const Simulation::CelestialBody& body, glm::vec3 parentPos) {
        glm::vec3 worldPos = solarSystem.isPhysicsEnabled() 
            ? (body.getPhysicsPosition() * sysScale) 
            : ((parentPos / sysScale + body.getPosition(simulationTime)) * sysScale);
        glm::vec4 clipPos = viewProj * glm::vec4(worldPos, 1.0f);
        bool isMoon = body.getParent() != nullptr;
        bool shouldShow = !isMoon || (body.getParent() == lockedBody || &body == lockedBody);
        if (clipPos.z > 0 && clipPos.w > 0 && shouldShow) {
            glm::vec3 ndc = glm::vec3(clipPos) / clipPos.w;
            float sx = (ndc.x + 1.0f) * 0.5f * sw, sy = (1.0f - ndc.y) * 0.5f * sh;
            ImU32 col = (hoveredBody == &body || selectedBody == &body) ? IM_COL32(255, 255, 255, 255) : IM_COL32(200, 200, 200, 150);
            drawList->AddText(ImVec2(sx + 10, sy - 10), col, body.getName().c_str());
        }
        for (const auto& child : body.getChildren()) drawLabel(*child, worldPos);
    };
    for (const auto& body : solarSystem.getBodies()) drawLabel(*body, glm::vec3(0.0f));
}

void SimulationUI::renderBodyTooltip(
    const Simulation::CelestialBody* body,
    const Simulation::CelestialBody* hoveredBody,
    const Simulation::CelestialBody* selectedBody,
    float mouseX, float mouseY,
    bool uiWantsMouse
) {
    if (!body || uiWantsMouse) return;
    
    ImGui::SetNextWindowBgAlpha(0.85f);
    if (hoveredBody) {
        ImGui::SetNextWindowPos(ImVec2(mouseX + 20, mouseY + 20), ImGuiCond_Always);
    } else {
        ImGui::SetNextWindowPos(ImVec2(10, 100), ImGuiCond_FirstUseEver);
    }
    
    ImGui::Begin("Body Info", nullptr, ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
    ImGui::TextColored(ImVec4(0.4, 0.8, 1.0, 1.0), "%s", body->getName().c_str());
    ImGui::Separator();
    
    if (body->isStar()) {
        ImGui::Text("Type: Star / Singularity");
        ImGui::Text("Radius: %.1f Earth Units", body->getRadius());
    } else {
        const char* typeStr = "Planet";
        if (body->getType() == Simulation::BodyType::Moon) typeStr = "Moon";
        else if (body->getType() == Simulation::BodyType::DwarfPlanet) typeStr = "Dwarf Planet";
        else if (body->getType() == Simulation::BodyType::Asteroid) typeStr = "Asteroid";
        ImGui::Text("Type: %s", typeStr);
        const auto& op = body->getOrbitalParams();
        ImGui::Text("Distance: %.3f AU", op.semiMajorAxis);
        ImGui::Text("Period: %.3f Years", op.orbitalPeriod);
    }
    
    if (hoveredBody && hoveredBody != selectedBody) {
        ImGui::TextColored(ImVec4(0.7, 0.7, 0.7, 1.0), "(Click to select)");
    } else if (body == selectedBody) {
        ImGui::TextColored(ImVec4(0.7, 0.7, 1.0, 1.0), "(Double-click to focus)");
    }
    ImGui::End();
}

} // namespace Render
