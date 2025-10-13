FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    g++ \
    libncurses-dev \
    gdb \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app