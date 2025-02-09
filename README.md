# computer-graphics-project-2022
A monster truck driving simulator developed in Vulkan with procedural terrain generation and rigidbody physics.
Tested on Linux and wayland.
Can be built using Zig or Cmake.

https://user-images.githubusercontent.com/36551215/180194323-7e0be32a-dd07-415b-b78d-4b3e357a8718.mp4

## Development Environment

This project can be developed using Visual Studio Code with the provided dev container. The container includes all necessary dependencies for building with both Zig and CMake.

## Compile with Zig (Recommended)

1. Run `zig build` in the project root
2. Built binary will be in `zig-out/bin/CGProject`

## Compile with CMake

1. Create build directory: `mkdir build && cd build`
2. Configure: `cmake -G Ninja ..`
3. Build: `ninja`
4. Built binary will be in `build/CGProject`

## Run the Project

### Using Zig Build:
```bash
zig build run
# or 
zig-out/bin/CGProject
```

### Using CMake Build:
```bash
build/CGProject
```

