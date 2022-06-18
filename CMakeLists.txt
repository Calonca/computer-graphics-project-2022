set(PNAME CGProject)
set(CFILE Project)
SET(CMAKE_BUILD_TYPE Debug) #Enable this to enable breakpoints

cmake_minimum_required(VERSION 3.18)
project(${PNAME})               # Create project PNAME
set(CMAKE_CXX_STANDARD 20)            # c++ version

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/headers)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/shaders)

# Build project, give it a name and includes list of file to be compiled
add_executable(${PNAME} src/${CFILE}.cpp
        src/gameObjects/Object.cpp
        src/gameObjects/Object.hpp
        src/gameObjects/Truck.cpp
        src/gameObjects/Truck.hpp
        src/utils/definitions.hpp
        src/utils/definitions.cpp
        src/utils/MatrixUtils.cpp
        src/utils/MatrixUtils.hpp
        src/utils/Model.hpp
        src/physics/PhysicsEngine.cpp
        src/physics/PhysicsEngine.hpp
        src/worldgen/Terrain.hpp
        src/worldgen/Terrain.cpp
        src/worldgen/Noise.hpp
        src/Project.hpp
        )

#target_compile_definitions (${PNAME} PUBLIC NDEBUG) #Used to deactivate validation layers messages

target_link_libraries(${PNAME} -lglfw -lvulkan -ldl -lpthread -lX11 -lXrandr -lXi)