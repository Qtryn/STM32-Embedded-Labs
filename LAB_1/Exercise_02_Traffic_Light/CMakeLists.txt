cmake_minimum_required(VERSION 3.22)

project(Exercise_02_Traffic_Light C ASM)


# =========================
# MCU Configuration
# =========================

set(MCU cortex-m3)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)


# =========================
# Compiler options
# =========================

add_compile_options(
    -mcpu=${MCU}
    -mthumb
    -Og
    -g3
    -Wall
    -ffunction-sections
    -fdata-sections
)


add_compile_definitions(
    USE_HAL_DRIVER
    STM32F103x6
)


# =========================
# Include directories
# =========================

include_directories(

    Core/Inc

    Drivers/CMSIS/Include

    Drivers/CMSIS/Device/ST/STM32F1xx/Include

    Drivers/STM32F1xx_HAL_Driver/Inc

    Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
)


# =========================
# Source files
# =========================

file(GLOB_RECURSE SOURCES

    Core/Src/*.c

    Drivers/STM32F1xx_HAL_Driver/Src/*.c

)


# =========================
# Executable
# =========================

add_executable(
    ${PROJECT_NAME}.elf

    ${SOURCES}

    startup_stm32f103x6.s
)


# =========================
# Linker
# =========================

target_link_options(
    ${PROJECT_NAME}.elf

    PRIVATE

    -mcpu=${MCU}

    -mthumb

    "-T${CMAKE_CURRENT_SOURCE_DIR}/STM32F103xx_FLASH.ld"

    --specs=nano.specs

    -Wl,--gc-sections

    -Wl,-Map=${PROJECT_NAME}.map
)


# =========================
# Generate HEX + BIN
# =========================

add_custom_command(

    TARGET ${PROJECT_NAME}.elf

    POST_BUILD

    COMMAND ${CMAKE_OBJCOPY}

    -O ihex

    $<TARGET_FILE:${PROJECT_NAME}.elf>

    ${PROJECT_NAME}.hex


    COMMAND ${CMAKE_OBJCOPY}

    -O binary

    $<TARGET_FILE:${PROJECT_NAME}.elf>

    ${PROJECT_NAME}.bin


    COMMENT "Generating HEX and BIN files"

)