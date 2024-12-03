FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    build-essential \
    gdb \
    cmake \
    gcc \
    build-essential \
    vim \
    openssh-server \
    sysstat \
    linux-tools-generic \
    strace \
    && apt-get clean

RUN alias perf=$(find /usr/lib/linux-tools/*/perf | head -1)

WORKDIR /workspace
