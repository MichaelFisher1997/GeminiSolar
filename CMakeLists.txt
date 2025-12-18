cmake_minimum_required(VERSION 3.20)
project(SolarSystemSim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable helpful warnings
add_compile_options(-Wall -Wextra -Wpedantic)

find_package(SDL3 REQUIRED)
find_package(glm REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# Collect source files
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "src/*.hpp"
    "src/imgui/*.cpp"
    "src/imgui/*.h"
    "src/imgui/backends/*.cpp"
    "src/imgui/backends/*.h"
)

add_executable(space_sim ${SOURCES})

target_include_directories(space_sim PRIVATE 
    src 
    src/imgui 
    src/imgui/backends 
    ${GLEW_INCLUDE_DIRS}
)

target_link_libraries(space_sim PRIVATE 
    SDL3::SDL3 
    glm::glm
    OpenGL::GL
    GLEW::GLEW
)

# Copy shader files to build directory for runtime loading
file(GLOB SHADER_FILES "assets/shaders/planet.*")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/assets/shaders")
foreach(SHADER ${SHADER_FILES})
    get_filename_component(FILENAME ${SHADER} NAME)
    configure_file(${SHADER} "${CMAKE_BINARY_DIR}/assets/shaders/${FILENAME}" COPYONLY)
endforeach()

# Copy system JSON files to build directory for runtime loading
file(GLOB SYSTEM_FILES "assets/systems/*.json")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/assets/systems")
foreach(SYSTEM ${SYSTEM_FILES})
    get_filename_component(FILENAME ${SYSTEM} NAME)
    configure_file(${SYSTEM} "${CMAKE_BINARY_DIR}/assets/systems/${FILENAME}" COPYONLY)
endforeach()
