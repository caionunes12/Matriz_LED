# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Configuração específica para Wokwi
set(PICO_SDK_PATH "/home/wokwi/pico-sdk")
set(PICO_BOARD pico_w)
set(PICO_PLATFORM rp2040)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(pio_matrix C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(pio_matrix
    pio_matrix.c
    animacoes.c
)

pico_set_program_name(pio_matrix "pio_matrix")
pico_set_program_version(pio_matrix "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(pio_matrix 0)
pico_enable_stdio_usb(pio_matrix 1)

# Configuração do PIO
pico_generate_pio_header(pio_matrix ${CMAKE_CURRENT_LIST_DIR}/pio_matrix.pio)

target_sources(pio_matrix PRIVATE pio_matrix.c)

# Add the standard library to the build
target_link_libraries(pio_matrix PRIVATE
        pico_stdlib
        hardware_pio
	    hardware_adc
        pico_bootrom)

# Add the standard include files to the build
target_include_directories(pio_matrix PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

# Add any user requested libraries
target_link_libraries(pio_matrix 
        
        )

pico_add_extra_outputs(pio_matrix)

