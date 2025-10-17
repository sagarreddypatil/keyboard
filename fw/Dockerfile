FROM debian:trixie

RUN apt-get update
RUN apt-get install -y build-essential
RUN apt-get install -y gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib
RUN apt-get install -y cmake ninja-build
RUN apt-get install -y python3
RUN apt-get install -y picotool

WORKDIR /workdir