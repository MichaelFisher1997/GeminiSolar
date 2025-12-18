#include "core/App.hpp"
#include "core/Logger.hpp"
#include <csignal>
#include <exception>

// Forward declaration from App.cpp
namespace Core {
    void requestGlobalShutdown();
}

namespace {
    void signalHandler(int signal) {
        // Request graceful shutdown - this is safe because we're using atomics
        Core::requestGlobalShutdown();
        
        // Log which signal (note: this isn't strictly async-signal-safe but usually works)
        if (signal == SIGINT) {
            // User pressed Ctrl+C
        } else if (signal == SIGTERM) {
            // Termination requested
        }
    }
    
    void setupSignalHandlers() {
        struct sigaction sa{};
        sa.sa_handler = signalHandler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        
        sigaction(SIGINT, &sa, nullptr);
        sigaction(SIGTERM, &sa, nullptr);
    }
}

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    setupSignalHandlers();
    LOG_INFO("Main", "Starting Solar System Simulator...");

    int exitCode = 0;
    
    try {
        Core::App app;
        app.run();
        LOG_INFO("Main", "Application exited normally");
    } catch (const std::exception& e) {
        LOG_FATAL("Main", "Fatal error: ", e.what());
        exitCode = 1;
    } catch (...) {
        LOG_FATAL("Main", "Unknown fatal error");
        exitCode = 1;
    }
    
    LOG_INFO("Main", "Cleanup complete, exiting with code ", exitCode);
    return exitCode;
}
