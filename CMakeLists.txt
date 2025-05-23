cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PICO_BOARD pico_w CACHE STRING "Board type")
include(pico_sdk_import.cmake)

# Define o nome do projeto como Teste_ADC_Display, suportando C, C++ e Assembly.
project(Ohmmeter C CXX ASM) 

pico_sdk_init()

include_directories(${CMAKE_SOURCE_DIR}/lib)

# Automatically gather all source files from src/
file(GLOB_RECURSE SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.c)

# Add executable. Default name is the project name, version 0.1
add_executable(
    ${PROJECT_NAME} 
    ${PROJECT_NAME}.c 
    ${SRC_FILES}
)

target_compile_definitions(

    ${PROJECT_NAME} PRIVATE 
    PICO_PRINTF_SUPPORT_FLOAT=1 
    PICO_STDIO_ENABLE_PRINTF=1
    
)

target_link_libraries(
    ${PROJECT_NAME} 
    pico_stdlib 
    hardware_i2c
    hardware_adc
    hardware_pio
)

pico_generate_pio_header(
    ${PROJECT_NAME} 
    ${CMAKE_CURRENT_LIST_DIR}/src/pio_matrix.pio)

# Add the standard include files to the build
target_include_directories(
    ${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

pico_add_extra_outputs(${PROJECT_NAME})
