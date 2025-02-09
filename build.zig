const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // Compile shaders
    const compileShaders = b.addSystemCommand(&.{"bash","compile.sh"});
    compileShaders.setCwd(b.path("shaders"));
    compileShaders.step.name = "compile shaders";

    // Create the game executable.
    const exe = b.addExecutable(.{
        .name = "CGProject",
        .target = target,
        .optimize = optimize,
    });

    // Add extra source files
    const sources = .{
        "src/Project.cpp",
        "src/gameObjects/Object.cpp",
        "src/gameObjects/Truck.cpp",
        "src/utils/definitions.cpp",
        "src/utils/MatrixUtils.cpp",
        "src/physics/PhysicsEngine.cpp",
        "src/worldgen/Terrain.cpp",
        "src/worldgen/Tree.cpp",
    };
    exe.addCSourceFiles(.{
        .files = &sources,
        .flags = &.{ "-std=c++20", "-O3" },
    });

    // Add include directories
    exe.addIncludePath(.{ .cwd_relative = "headers" });
    exe.addIncludePath(.{ .cwd_relative = "shaders" });

    // Add pkg-config packages for Wayland and EGL.
    exe.linkLibCpp();
    exe.linkSystemLibrary("wayland-egl");
    exe.linkSystemLibrary("glfw");
    exe.linkSystemLibrary("vulkan");
    exe.linkSystemLibrary("dl");
    exe.linkSystemLibrary("pthread");
    exe.linkSystemLibrary("Xrandr");
    exe.linkSystemLibrary("Xi");
    exe.step.dependOn(&compileShaders.step);

    b.installArtifact(exe);

    // Add a run step to run the executable. Can be called with `zig build run`
    const run_exe = b.addRunArtifact(exe);
    const run_step = b.step("run", "Run the application");

    run_step.dependOn(&run_exe.step);
}
