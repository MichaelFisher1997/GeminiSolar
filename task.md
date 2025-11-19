# SPACE_SIM_SPEC.md

## 1. Project Overview

Build a real-time Solar System simulator using **C++**, **SDL3**, and **Vulkan**, with builds and dev environment managed by a **Nix flake**.

The application should:

- Render the Sun and all 8 major planets orbiting in 3D.
- Use an approximate but **physically-motivated** orbital model (elliptical orbits, correct relative orbital periods, inclinations simplified but not zero).
- Provide a free-fly camera and “focus on body” modes.
- Support variable time scale (pause → real time → fast-forward).
- Run full-screen or windowed on Linux (NixOS) by default.

Non-goals (for v1):

- No need for photorealistic textures; simple shaded spheres with basic materials is fine.
- No need for exact JPL ephemeris precision; relative correctness and smooth motion is enough.
- No need for complex UI toolkit; minimal on-screen overlays/text is fine.

---

## 2. Tech Stack & Dependencies

### 2.1 Languages & Core Tech

- **Language:** C++20
- **Window & input:** SDL3
- **Rendering:** Vulkan
- **Math:** glm (or similar), via Nix
- **Build system:** CMake (preferred) or Meson, but pick one and stick to it.
- **Dependency & environment management:** Nix flake

### 2.2 Nix Flake Requirements

Create a Nix flake that provides:

- A `devShell` with:
  - C++ toolchain (clang/llvm or gcc)
  - `cmake` (or chosen build system)
  - `SDL3` development headers
  - Vulkan SDK (loader, headers, validation layers)
  - `glm`
  - `ninja` if needed
- A `packages.default` that builds the release binary for the app.
- Simple `nix develop` + `cmake .. && make` (or equivalent) workflow.

The flake should be the single source of truth for all dependencies.

---

## 3. High-Level Features

1. **Solar System Model**
   - Sun + 8 planets (Mercury → Neptune).
   - Each body has:
     - Name
     - Radius (relative scale, clamped so smaller bodies are still visible)
     - Orbital radius (semi-major axis, scaled down)
     - Orbital period (correct relative ordering & ratios)
     - Orbital eccentricity (can be small but non-zero)
     - Orbital inclination (small angles so orbits are slightly tilted)
     - Color / material parameters

2. **Orbital Simulation**
   - Planets orbit the Sun along **elliptical** paths in a fixed plane (plus small inclinations).
   - Use a **simple analytic model**, not an N-body integrator:
     - Time parameter `t` → mean anomaly → angle around the Sun.
     - Approximate eccentric orbits; can use a simplified formula, no need for full Kepler solver.
   - Allow **global time scale** control:
     - Paused
     - Real-time (1 day ≈ 1 second configurable)
     - Fast-forward (10×, 100×, 1000×)
   - All orbital positions are derived from **simulation time**, not frame time.

3. **Rendering**
   - 3D perspective view.
   - Sun at origin (0,0,0).
   - Planets rendered as colored spheres using simple lighting (Phong/Blinn–Phong or similar).
   - Simple starfield background (e.g., static cube or skybox with star texture, or procedural points).
   - Clear, smooth camera controls for exploring the system.

4. **Camera & Controls**
   - **Free-fly camera**:
     - WASD for movement in camera plane.
     - Space / Ctrl (or Q/E) to move up/down.
     - Mouse right-button drag to look around.
   - **Focus mode:**
     - Keypress to cycle focus between bodies (Sun, Mercury, Venus, …).
     - When focused:
       - Camera orbits around the selected body at a configurable distance.
       - Option to “follow orbital motion” so the camera tracks the planet.
   - **Time controls:**
     - Keys for pause/resume.
     - Keys for increasing/decreasing time scale.
     - Display current time scale as HUD text.

5. **UI / HUD**
   - Minimal overlay (can be simple text rendered via bitmap font or imgui-like debug overlay if convenient).
   - Display:
     - Selected body name
     - Simulation time (in days/years from t=0)
     - Current time scale multiplier
     - Basic control hints (short list)

6. **Performance & Quality**
   - Ensure it runs smoothly on a modest GPU at 1080p.
   - Use Vulkan best practices:
     - Command buffers
     - Descriptor sets
     - Vertex/index buffers
   - Keep architecture modular so future features (moons, asteroid belts) can be added without major refactor.

---

## 4. Architecture

Use a clean, modular C++ architecture. Prioritize clarity and separation of concerns.

### 4.1 Project Structure

Use a structure like:

- `flake.nix`
- `CMakeLists.txt`
- `src/`
  - `main.cpp`
  - `core/`
    - `App.hpp/cpp` (high-level application lifecycle)
    - `Time.hpp/cpp` (simulation time management)
  - `platform/`
    - `SDLWindow.hpp/cpp` (window & input)
  - `render/`
    - `VulkanContext.hpp/cpp`
    - `Swapchain.hpp/cpp`
    - `Renderer.hpp/cpp`
    - `Camera.hpp/cpp`
    - `Mesh.hpp/cpp`
    - `Material.hpp/cpp`
    - `UniformBuffers.hpp/cpp`
  - `simulation/`
    - `CelestialBody.hpp/cpp`
    - `SolarSystem.hpp/cpp`
    - `OrbitModel.hpp/cpp`
  - `ui/`
    - `HUD.hpp/cpp` (simple overlay text, debug UI)
- `assets/`
  - `shaders/` (GLSL)
  - `textures/` (optional)
- `README.md`

Adjust paths as needed, but keep logical separation.

### 4.2 Core Components

1. **App**
   - Handles:
     - Initialization (Nix environment assumed already set up).
     - SDL3 window + Vulkan context setup.
     - Main loop (event processing, time update, simulation update, render).
   - Clean shutdown of Vulkan resources & SDL.

2. **Time System**
   - Maintain:
     - `double simulationTime` (in days or seconds since t=0)
     - `double timeScale` (multiplier)
     - `bool paused`
   - `update(deltaRealSeconds)`:
     - If not paused: `simulationTime += deltaRealSeconds * timeScale`.

3. **Solar System & Orbital Model**
   - `CelestialBody`:
     - Data: name, radius, color, orbital params.
     - Function: `glm::vec3 getPosition(double simulationTime)` returning world-space position.
   - `SolarSystem`:
     - Holds Sun + all planets.
     - Provides iteration for rendering.
   - `OrbitModel`:
     - Utility to compute positions from orbital parameters.
     - Supports eccentricity and inclination (simplified).

4. **Rendering Layer**
   - `VulkanContext`:
     - Instance, physical device, logical device, queues.
     - Debug layers (if debug build).
   - `Swapchain`:
     - Swapchain + image views.
   - `Renderer`:
     - Create pipelines:
       - One pipeline for planet spheres.
       - Optional pipeline for orbit lines (simple line strips).
     - Manage depth buffer.
     - Manage uniform buffers (per-frame view/projection matrices, per-body model matrices).
   - `Camera`:
     - Position, orientation, projection.
     - Methods for:
       - Free-fly movement.
       - Focus on a specific target with orbital offset.
   - Meshes:
     - Generate sphere mesh procedurally at startup (shared by all planets, scaled per-instance).
     - Simple line meshes for orbit paths.

5. **Input & Control System**
   - Use SDL3 for:
     - Keyboard input events.
     - Mouse motion & button events.
   - Map inputs:
     - Movement (WASD, etc.)
     - Camera look (mouse)
     - Time controls (+/-, P)
     - Focus cycle (e.g., Tab to cycle body)
   - Implement a small input abstraction so Renderer/Simulation don’t depend directly on SDL.

6. **UI / HUD**
   - Minimum viable:
     - Simple text draw with a fixed font.
     - Can use a bitmap/font library or a very small text rendering system.
   - Show:
     - Body name, distance from Sun, orbital period (simple computed text).
     - Current time scale.
     - Paused/Playing indicator.
   - Optional: Add a simple debug console overlay toggled by key.

---

## 5. Orbital Data & Scaling

### 5.1 Data

Hardcode approximate orbital parameters in a `SolarSystem.cpp` file:

For each planet:
- Semi-major axis (AU→scaled units).
- Orbital period (Earth years→seconds of simulation).
- Eccentricity.
- Inclination (a few degrees).
- Radius (Earth radii → clamped/scaled).

Values don’t have to be perfect but should:
- Preserve correct ordering.
- Preserve rough ratios (e.g., Jupiter orbits slower than Earth, Mercury fastest, etc.).
- Keep bodies visible and not all bunched together.

### 5.2 Scaling Rules

Define global constants:
- `DISTANCE_SCALE` – maps AU to scene units.
- `RADIUS_SCALE` – magnifies small planets so they aren’t invisible.
- Guarantee that:
  - Inner planets are clearly separable visually.
  - Planets do not intersect the Sun’s mesh visually.

---

## 6. Rendering Details

- Use a single primary render pass:
  - Clear color to dark space-like color.
  - Draw star background first (optional skybox).
  - Draw orbit lines.
  - Draw Sun and planets.
- Lighting:
  - One directional or point light at Sun’s position.
  - Simple diffuse + specular shading.
- Depth testing enabled.
- Correct aspect ratio handling on window resize.

---

## 7. Controls (Exact Behavior)

Implement the following key bindings (can be changed but must exist):

- Movement:
  - `W/S` – forward/backward
  - `A/D` – left/right
  - `Q/E` – down/up (or Space/Ctrl)
- Camera:
  - Right mouse button held + move mouse → look around.
- Time:
  - `P` – toggle pause.
  - `=` or `+` – increase time scale (up to e.g. 1000×).
  - `-` – decrease time scale (down to 0.1×).
- Focus:
  - `Tab` – cycle through Sun → Mercury → … → Neptune → Sun…
  - `F` – toggle “follow mode” for the currently focused body (camera tracks body movement).
- Misc:
  - `Esc` – quit.

Display these bindings in a small “Help” overlay accessible via e.g. `H`.

---

## 8. Code Quality & Structure

- Use C++20 features where appropriate (structured bindings, `std::variant` if needed, `std::optional`, etc.).
- Keep headers clean; avoid massive header bloat.
- Separate interface and implementation into `.hpp` / `.cpp`.
- No global mutable state; pass state via objects and references.
- Make it easy to add more bodies (e.g., moons) later.

---

## 9. Build & Run

Implement:

- `README.md` with:
  - How to enter dev shell: `nix develop`.
  - How to configure and build: `cmake -S . -B build && cmake --build build`.
  - How to run: `./build/space_sim` (or equivalent).
- Optional: Nix `apps` entry to run via `nix run`.

---

## 10. Testing & Debugging

- Add a debug mode (compile-time flag or runtime flag) that:
  - Enables Vulkan validation layers.
  - Prints key simulation data (time scale, positions) to the console occasionally.
- Basic sanity checks:
  - Time scale changes affect orbital speed.
  - Pausing stops orbital motion.
  - Focus mode orbits the selected body around its center at a fixed radius.

---

## 11. Extensibility Notes (Future Work, but Keep in Mind)

Design the architecture so it will be easy later to add:

- Major moons (e.g., Earth’s Moon, Galilean moons).
- Very simple N-body approximation or perturbations.
- More sophisticated materials and textures.
- Simple menu for toggling visual layers (orbits on/off, labels on/off).

You don’t need to implement these now, but keep classes generic enough that adding them is straightforward.

