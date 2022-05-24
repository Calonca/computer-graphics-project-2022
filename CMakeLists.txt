set(PNAME CGProject)
set(CFILE Project)
SET(CMAKE_BUILD_TYPE Debug) #Enable this to enable breakpoints

cmake_minimum_required(VERSION 3.18)
project(${PNAME})               # Create project PNAME
set(CMAKE_CXX_STANDARD 20)            # Enable c++17 standard

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/headers)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/shaders)

# Build project, give it a name and includes list of file to be compiled
add_executable(${PNAME} ${CFILE}.cpp
        Component.cpp
        Component.h
        definitions.h
        PhysicsEngine.cpp
        PhysicsEngine.h
        Truck.cpp
        Truck.h
        PerlinNoise.h
        PerlinNoise.cpp)

target_link_libraries(${PNAME} -lglfw -lvulkan -ldl -lpthread -lX11 -lXrandr -lXi)