cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(external/pico-sdk/pico_sdk_init.cmake)

project(kbfw C CXX)

pico_sdk_init()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(kbfw main.cc usb_descriptors.c)
target_include_directories(kbfw PUBLIC ./)
target_link_libraries(kbfw pico_stdlib hardware_gpio tinyusb_device tinyusb_board)
pico_enable_stdio_usb(kbfw 0)
pico_enable_stdio_uart(kbfw 0)

pico_set_binary_type(kbfw copy_to_ram)
pico_add_uf2_output(kbfw)
