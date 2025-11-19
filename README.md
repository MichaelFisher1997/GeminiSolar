# Solar System Simulator

A real-time 3D Solar System simulator built with C++, SDL3, and Vulkan.

## Requirements

- [Nix](https://nixos.org/download.html) with flakes enabled.

## Development

1. Enter the development environment:
   ```bash
   nix develop
   ```

2. Build the project:
   ```bash
   cmake -S . -B build
   cmake --build build
   ```

3. Run the simulator:
   ```bash
   ./build/space_sim
   ```

## Controls

- **WASD**: Move Camera
- **Q/E**: Move Down/Up
- **Right Mouse Drag**: Look around
- **Tab**: Cycle focus (Sun -> Planets)
- **F**: Toggle focus follow mode
- **P**: Pause/Resume
- **+/-**: Increase/Decrease time scale
