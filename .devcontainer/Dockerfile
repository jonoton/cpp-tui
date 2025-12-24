FROM mcr.microsoft.com/devcontainers/cpp:1-debian-12

# Install minimal additional tools if needed
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install --no-install-recommends \
    cmake \
    build-essential \
    gdb \
    && apt-get clean -y && rm -rf /var/lib/apt/lists/*
