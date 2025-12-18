# AGENTS.md - Solar System Simulator

## Build Commands
```bash
nix develop                    # Enter dev environment (required first)
cmake -S . -B build            # Configure
cmake --build build            # Build
./build/space_sim              # Run
```

## Project Info
- **Language**: C++20 with `-Wall -Wextra -Wpedantic`
- **Dependencies**: SDL3, OpenGL, GLEW, GLM, ImGui (vendored in src/imgui/)
- **No tests configured** - this is a simulation/graphics project

## Architecture (SOLID-compliant)
- **Core**: App (orchestrator), InputManager, Time, Logger
- **Platform**: WindowInterface (abstract), SDLWindow (concrete)
- **Render**: RenderInterface (abstract), GLRenderer, ShaderManager, MeshFactory, UIManager, Camera
- **Simulation**: SolarSystem, CelestialBody, OrbitModel

## Code Style
- **Namespaces**: PascalCase matching directory (e.g., `Core::`, `Render::`, `Simulation::`)
- **Classes/Structs**: PascalCase (e.g., `CelestialBody`, `OrbitalParams`)
- **Member variables**: `m_camelCase` prefix (e.g., `m_window`, `m_isRunning`)
- **Methods**: camelCase (e.g., `getPosition()`, `createBuffer()`)
- **Headers**: Use `#pragma once`, project includes before std includes
- **Pointers**: Prefer `std::unique_ptr<>` for ownership, raw pointers for non-owning refs
- **Error handling**: Exceptions with try/catch at top level, use LOG_* macros for logging
- **Comments**: `//` for inline, units in comments (e.g., `// AU`, `// Radians`)
