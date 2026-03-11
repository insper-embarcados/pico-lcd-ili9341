# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
cmake_minimum_required(VERSION 3.13)

# =========================
# Configuração padrão Pico
# =========================

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Board
set(PICO_BOARD pico2 CACHE STRING "Board type")

include(pico_sdk_import.cmake)

project(ili9341 C CXX ASM)

pico_sdk_init()

# =========================
# Executável
# =========================

add_executable(ili9341
    main.c

    # ILI9341 driver
    tft_lcd_ili9341/ili9341/ili9341.c

    # GFX engine
    tft_lcd_ili9341/gfx/gfx_ili9341.c

    # Touch
    tft_lcd_ili9341/touch_resistive/touch_resistive.c
)

# =========================
# Includes
# =========================

target_include_directories(ili9341 PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/tft_lcd_ili9341/gfx
    ${CMAKE_CURRENT_LIST_DIR}/tft_lcd_ili9341/ili9341
    ${CMAKE_CURRENT_LIST_DIR}/tft_lcd_ili9341/touch_resistive
)

# =========================
# Bibliotecas
# =========================

target_link_libraries(ili9341
    pico_stdlib
    hardware_spi
    hardware_adc
)

# =========================
# STDIO
# =========================

pico_enable_stdio_uart(ili9341 1)
pico_enable_stdio_usb(ili9341 0)

# =========================
# Outputs
# =========================

pico_add_extra_outputs(ili9341)