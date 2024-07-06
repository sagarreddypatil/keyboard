cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)

# ---------- Pico SDK Project Setup  ----------
# Importing the pico-sdk importer
include(external/pico-sdk/pico_sdk_init.cmake)

# Project
project(KB_Firmware)
pico_sdk_init()

set(stdlibs pico_stdlib hardware_gpio hardware_spi hardware_dma hardware_uart)

# so that the build folder doesn't get cluttered
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
    -Wall # Enable all warnings
    -O3 # I am speed
    -Werror # Treat warnings as errors
    -Wno-unused-variable # annoying
    -Wno-unused-but-set-variable
    -Wno-strict-aliasing # pico-sdk fails this
)

# --------- Git Describe ---------
execute_process(
    COMMAND git describe --tags --dirty
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_DESCRIBE_STRING
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# ---------- Program  ----------

file(GLOB_RECURSE srcs src/*.c)
file(GLOB_RECURSE hdrs include/*.h)

add_executable(app ${srcs} ${hdrs})
target_include_directories(app PUBLIC include)
target_link_directories(app PUBLIC src)
pico_set_program_name(app "KB_Firmware")
pico_set_program_version(app "${GIT_DESCRIBE_STRING} (build ${CMAKE_BUILD_TYPE})")

set_target_properties(app
    PROPERTIES

    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/elf
)

target_link_libraries(app ${stdlibs})

pico_enable_stdio_usb(app 1)
pico_enable_stdio_uart(app 0)

pico_set_binary_type(app copy_to_ram) # run full program from ram

# pico_set_float_implementation(app none)
pico_add_uf2_output(app)
