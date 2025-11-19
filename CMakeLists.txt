cmake_minimum_required(VERSION 3.20)
project(SolarSystemSim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable helpful warnings
add_compile_options(-Wall -Wextra -Wpedantic)

find_package(SDL3 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# Collect source files
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "src/*.hpp"
)

add_executable(space_sim ${SOURCES})

target_include_directories(space_sim PRIVATE src ${GLEW_INCLUDE_DIRS})

target_link_libraries(space_sim PRIVATE 
    SDL3::SDL3 
    Vulkan::Vulkan 
    glm::glm
    OpenGL::GL
    GLEW::GLEW
)

# Custom command to compile shaders (simple version)
find_program(GLSLC_EXECUTABLE glslc)
if(GLSLC_EXECUTABLE)
    message(STATUS "Found glslc: ${GLSLC_EXECUTABLE}")
    
    # Function to compile a shader
    function(compile_shader SHADER_FILE)
        get_filename_component(FILENAME ${SHADER_FILE} NAME)
        set(SPV_FILE "${CMAKE_BINARY_DIR}/shaders/${FILENAME}.spv")
        file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/shaders")
        
        add_custom_command(
            OUTPUT ${SPV_FILE}
            COMMAND ${GLSLC_EXECUTABLE} ${SHADER_FILE} -o ${SPV_FILE}
            DEPENDS ${SHADER_FILE}
            COMMENT "Compiling shader ${FILENAME}"
        )
        target_sources(space_sim PRIVATE ${SPV_FILE})
    endfunction()

    file(GLOB SHADER_FILES "assets/shaders/*")
    foreach(SHADER ${SHADER_FILES})
        compile_shader(${SHADER})
    endforeach()
else()
    message(WARNING "glslc not found! Shaders will not be compiled.")
endif()
