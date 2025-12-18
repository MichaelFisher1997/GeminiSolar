#include "UIManager.hpp"
#include "core/Logger.hpp"
#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_opengl3.h"
#include <SDL3/SDL.h>

namespace Render {

UIManager::UIManager(Platform::WindowInterface& window, void* glContext) 
    : m_window(window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    ImGui::StyleColorsDark();
    
    SDL_Window* sdlWindow = static_cast<SDL_Window*>(window.getNativeHandle());
    ImGui_ImplSDL3_InitForOpenGL(sdlWindow, glContext);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    LOG_INFO("UIManager", "ImGui initialized");
}

UIManager::~UIManager() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    LOG_INFO("UIManager", "ImGui shutdown");
}

void UIManager::beginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void UIManager::endFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool UIManager::wantsKeyboard() const {
    return ImGui::GetIO().WantCaptureKeyboard;
}

bool UIManager::wantsMouse() const {
    return ImGui::GetIO().WantCaptureMouse;
}

void UIManager::processEvent(const SDL_Event& event) {
    ImGui_ImplSDL3_ProcessEvent(&event);
}

} // namespace Render
