FROM fedora:latest

# Create username variable
ARG USERNAME=vscode

# Create a non-root user named '$USERNAME'
RUN useradd -m -s /bin/bash $USERNAME \
    && echo "$USERNAME ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME


# Set up Wayland environment variables
# ENV WAYLAND_DISPLAY=wayland-0
# ENV XDG_RUNTIME_DIR=/run/user/1000

RUN sudo dnf install -y \
    glfw glfw-devel \
    vulkan-devel vulkan-validation-layers glslc \
    wayland-devel \
    ninja-build cmake gcc-c++ gdb \
    util-linux-user \
    git zsh xdg-desktop-portal \
    zig

# Needs to be updated to work with wayland
RUN sudo dnf install -y \
https://kojipkgs.fedoraproject.org/packages/glfw/3.4/2.fc42/x86_64/glfw-3.4-2.fc42.x86_64.rpm \
https://kojipkgs.fedoraproject.org/packages/glfw/3.4/2.fc42/x86_64/glfw-devel-3.4-2.fc42.x86_64.rpm

