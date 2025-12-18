#include "SDLWindow.hpp"
#include "core/Logger.hpp"
#include <stdexcept>
#include <GL/glew.h>

namespace Platform {

SDLWindow::SDLWindow(const std::string& title, int width, int height) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        throw std::runtime_error("Failed to initialize SDL: " + std::string(SDL_GetError()));
    }

    // Set OpenGL attributes before window creation
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_WindowFlags window_flags = (SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    m_window = SDL_CreateWindow(
        title.c_str(),
        width,
        height,
        window_flags
    );

    if (!m_window) {
        SDL_Quit();
        throw std::runtime_error("Failed to create SDL window: " + std::string(SDL_GetError()));
    }
    
    LOG_INFO("SDLWindow", "Window created: ", width, "x", height);
}

SDLWindow::~SDLWindow() {
    if (m_glContext) {
        SDL_GL_DestroyContext(m_glContext);
    }
    if (m_window) {
        SDL_DestroyWindow(m_window);
    }
    SDL_Quit();
    LOG_INFO("SDLWindow", "Window destroyed");
}

void SDLWindow::createGLContext() {
    m_glContext = SDL_GL_CreateContext(m_window);
    if (!m_glContext) {
        throw std::runtime_error("Failed to create OpenGL context: " + std::string(SDL_GetError()));
    }
    
    SDL_GL_MakeCurrent(m_window, m_glContext);
    
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        throw std::runtime_error("Failed to initialize GLEW: " + std::string((const char*)glewGetErrorString(err)));
    }
    
    // Enable VSync
    SDL_GL_SetSwapInterval(1);
    
    LOG_INFO("SDLWindow", "OpenGL context created - Version: ", (const char*)glGetString(GL_VERSION));
}

void SDLWindow::getSize(int& width, int& height) const {
    SDL_GetWindowSize(m_window, &width, &height);
}

void SDLWindow::setTitle(const std::string& title) {
    SDL_SetWindowTitle(m_window, title.c_str());
}

void SDLWindow::swapBuffers() {
    SDL_GL_SwapWindow(m_window);
}

void SDLWindow::makeContextCurrent() {
    SDL_GL_MakeCurrent(m_window, m_glContext);
}

void SDLWindow::pollEvents(EventCallback callback) {
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent)) {
        WindowEvent event;
        
        switch (sdlEvent.type) {
            case SDL_EVENT_QUIT:
                event.type = WindowEventType::Quit;
                m_shouldClose = true;
                callback(event);
                break;
                
            case SDL_EVENT_WINDOW_RESIZED:
                event.type = WindowEventType::Resized;
                event.width = sdlEvent.window.data1;
                event.height = sdlEvent.window.data2;
                callback(event);
                break;
                
            case SDL_EVENT_KEY_DOWN:
                event.type = WindowEventType::KeyDown;
                event.keyCode = sdlEvent.key.key;
                event.scanCode = sdlEvent.key.scancode;
                callback(event);
                break;
                
            case SDL_EVENT_KEY_UP:
                event.type = WindowEventType::KeyUp;
                event.keyCode = sdlEvent.key.key;
                event.scanCode = sdlEvent.key.scancode;
                callback(event);
                break;
                
            case SDL_EVENT_MOUSE_MOTION:
                event.type = WindowEventType::MouseMotion;
                event.mouseX = sdlEvent.motion.x;
                event.mouseY = sdlEvent.motion.y;
                callback(event);
                break;
                
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                event.type = WindowEventType::MouseButtonDown;
                event.mouseButton = sdlEvent.button.button;
                event.mouseX = sdlEvent.button.x;
                event.mouseY = sdlEvent.button.y;
                callback(event);
                break;
                
            case SDL_EVENT_MOUSE_BUTTON_UP:
                event.type = WindowEventType::MouseButtonUp;
                event.mouseButton = sdlEvent.button.button;
                event.mouseX = sdlEvent.button.x;
                event.mouseY = sdlEvent.button.y;
                callback(event);
                break;
                
            case SDL_EVENT_MOUSE_WHEEL:
                event.type = WindowEventType::MouseWheel;
                event.wheelDelta = sdlEvent.wheel.y;
                callback(event);
                break;
        }
    }
}

} // namespace Platform
